//INCLUSÃO DE BIBLIOTECA
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h> 
#include "EmonLib.h"

// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

// Variaveis pinos
// variavel sensor temp
#define DHTPIN A0
// variavel IR
#define RECV_PIN 4
// variavel sensor luz
#define sensor_luz A1
// variaveis led rgb
#define red 6
#define green 7
#define blue 5
// variavel sensor corrente
const int pinoSensor = A2; 

// Variaveis rele
#define rele1 53
#define rele2 52
#define rele3 51
#define rele4 50
#define rele5 49
#define rele6 48
#define rele7 47
#define rele8 46

// Variaveis usb
#define usb1 45
#define usb2 44 
#define usb3 43 
#define usb4 42
#define usb5 39
#define usb6 38
#define usb7 37
#define usb8 36
#define usb9 34

// Variaveis sensor luz
int ldrValor = 0;

// Variaveis sensor temperatura
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
float t=0,h=0;

// // Variaveis sensor IR
IRrecv irrecv(RECV_PIN);  
decode_results results;  

// Variaveis sensor corrrente
#define CURRENT_CAL 18.40 
EnergyMonitor emon1;
double currentDraw,ruido = 0.07;
 
bool r1 = false,r2 = true,r3 = true,r4 = true,r5 = true,r6 = true,r7 = true,r8 = true;

void setup()
{  
  Serial.begin(9600);
  lcd.begin (16,2);
  dht.begin(); 
  irrecv.enableIRIn();
  
  pinMode(red, OUTPUT);   
  pinMode(green, OUTPUT); 
  pinMode(blue, OUTPUT); 

   
  pinMode(rele1, OUTPUT);   
  pinMode(rele2, OUTPUT); 
  pinMode(rele3, OUTPUT);  
  pinMode(rele4, OUTPUT);   
  pinMode(rele5, OUTPUT); 
  pinMode(rele6, OUTPUT);  
  pinMode(rele7, OUTPUT);   
  pinMode(rele8, OUTPUT); 

  
  pinMode(usb1, OUTPUT);   
  pinMode(usb2, OUTPUT); 
  pinMode(usb3, OUTPUT);  
  pinMode(usb4, OUTPUT);   
  pinMode(usb5, OUTPUT); 
  pinMode(usb6, OUTPUT);  
  pinMode(usb7, OUTPUT);   
  pinMode(usb8, OUTPUT); 
  pinMode(usb9, OUTPUT);  
  
  setOnOff(rele1,true);
  setOnOff(rele2,true);
  setOnOff(rele3,true);
  setOnOff(rele4,true);
  setOnOff(rele5,true);
  setOnOff(rele6,true);
  setOnOff(rele7,true);
  setOnOff(rele8,true);

  emon1.current(pinoSensor, CURRENT_CAL);

}
 
void loop()
{
  getPowerCurrent();
  getTempUmid();
  getLumi();
  getIrRemote();
  
  //setRele();
  if(ldrValor < 300)
    setUsb(true);
  else
    setUsb(false);
  
  showDisplay();
  delay(500);
}

void showDisplay(){
  
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("T");
  lcd.setCursor(2,0);
  lcd.print(t);
  lcd.setCursor(8,0);
  lcd.print("L");
  lcd.setCursor(10,0);
  lcd.print(ldrValor);
  lcd.setCursor(0,1);
  lcd.print("U");
  lcd.setCursor(2,1);
  lcd.print(h);
  lcd.setCursor(8,1);
  lcd.print("C");
  lcd.setCursor(10,1);
  lcd.print(currentDraw);
  
}

void getTempUmid(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  //Serial.println(h);
  //Serial.println(t);
}

void getLumi(){
  ldrValor = analogRead(sensor_luz);
  //Serial.println(ldrValor);
}

void getIrRemote(){
  
  if (irrecv.decode(&results))  {  
    Serial.print("Valor lido : ");  
    Serial.println(results.value, HEX);  
    float armazenavalor = (results.value);
    if (armazenavalor == 0xFFA25D){  
      brightnessRed();
    }else if (armazenavalor == 0xFF629D){   
      brightnessGreen(); 
    }else if (armazenavalor == 0xFFE21D){   
      brightnessBlue();
    }else if (armazenavalor == 0xFF22DD){   
      brightnessPurple(); 
    }else if (armazenavalor == 0xFF02FD){   
      brightnessWhite(); 
    }else if (armazenavalor == 0xFFC23D){   
      brightnessYellow(); 
    }else if (armazenavalor == 0xFF38C7){   
      brightnessOff(); 
    }else if (armazenavalor == 0xFFE01F){ 
      r1 = !r1;
      setOnOff(rele1,r1); 
    }else if (armazenavalor == 0xFFA857){
      r2 = !r2;  
      setOnOff(rele2,r2);
    }else if (armazenavalor == 0xFF906F){ 
      r3 = !r3;
      setOnOff(rele3,r3);
    }else if (armazenavalor == 0xFF6897){ 
      r4 = !r4;
      setOnOff(rele4,r4);
    }else if (armazenavalor == 0xFF9867){ 
      r5 = !r5;
      setOnOff(rele5,r5);
    }else if (armazenavalor == 0xFFB04F){
      r6 = !r6;
      setOnOff(rele6,r6);
    }else if (armazenavalor == 0xFF18E7){ 
      r7 = !r7;
      setOnOff(rele7,r7);
    }else if (armazenavalor == 0xFF4AB5){ 
      r8 = !r8; 
      setOnOff(rele8,r8);
    }  
    irrecv.resume();
  }
}

void setOnOff(int pin, bool choice){
  if(choice)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

void setRele(){
  
  for(int i = rele1; i>=rele8;i--){
  Serial.print("aqui ");
    setOnOff(i,false);
    delay(1000);
    }
    
  
  setOnOff(rele1,true);
  setOnOff(rele2,true);
  setOnOff(rele3,true);
  setOnOff(rele4,true);
  setOnOff(rele5,true);
  setOnOff(rele6,true);
  setOnOff(rele7,true);
  setOnOff(rele8,true);
  
  /*
  setOnOff(rele1,false);
  setOnOff(rele2,false);
  setOnOff(rele3,false);
  setOnOff(rele4,false);
  setOnOff(rele5,true);
  setOnOff(rele6,true);
  setOnOff(rele7,true);
  setOnOff(rele8,true);*/
}

void setUsb(bool option){
  Serial.println(option)
  setOnOff(usb1,option);
  setOnOff(usb2,option);
  setOnOff(usb3,option);
  setOnOff(usb4,option);
  setOnOff(usb5,option);
  setOnOff(usb6,option);
  setOnOff(usb7,option);
  setOnOff(usb8,option);
  setOnOff(usb9,option);
}

void getPowerCurrent(){
  emon1.calcVI(17, 100);
  currentDraw = emon1.Irms; 
  currentDraw-=ruido; 
  if(currentDraw < 0){ 
      currentDraw = 0;
  }
 
  Serial.print("Corrente medida: ");
  Serial.print(currentDraw); 
  Serial.println("A");
}
 
//Funções responsáveis por executar o brilho selecionado
void brightnessRed(){
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

void brightnessBlue(){
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, HIGH);
}

void brightnessGreen(){
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  digitalWrite(blue, LOW);
}

void brightnessPurple(){
  analogWrite(red, 255);
  analogWrite(green, 0);
  analogWrite(blue, 207);
}

void brightnessWhite(){
  digitalWrite(red, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);
}

void brightnessYellow(){
  analogWrite(red, 255);
  analogWrite(green, 50);
  analogWrite(blue, 0);
}

void brightnessOff(){
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  }
