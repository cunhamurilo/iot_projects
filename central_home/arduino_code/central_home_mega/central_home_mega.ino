//INCLUSÃO DE BIBLIOTECA
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h> 
#include "EmonLib.h"
#include <SoftwareSerial.h> 

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
#define button 3
 
// variavel sensor corrente
const int pinoSensor = A8; 

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
EnergyMonitor emon1;        // Cria uma instância
#define CURRENT_CAL 22.40   //VALOR DE CALIBRAÇÃO
float ruido = 0.4;         //RUÍDO PRODUZIDO NA SAÍDA DO SENSOR
double currentDraw = 0;

String day = "", hour ="", month = "";

String daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

unsigned long millisLcd = 0;
bool showHour = true;

int distance = 20;

void setup(){
    
  Serial.begin(115200);
  Serial1.begin(115200); // inicia a comunicação serial com o ESP8266
  
  lcd.begin (16,2);
  dht.begin(); 
  irrecv.enableIRIn();
  
  pinMode(red, OUTPUT);   
  pinMode(green, OUTPUT); 
  pinMode(blue, OUTPUT); 
  pinMode(button,INPUT);
   
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
  
  setOnOff(rele1,false);
  setOnOff(rele2,false);
  setOnOff(rele3,false);
  setOnOff(rele4,false);
  setOnOff(rele5,false);
  setOnOff(rele6,false);
  setOnOff(rele7,false);
  setOnOff(rele8,false);

  //emon1.current(pinoSensor, CURRENT_CAL);
  emon1.current(pinoSensor, CURRENT_CAL);
  
  millisLcd = millis();
}
 
void loop()
{
  getPowerCurrent();
  getTempUmid();
  getLumi();
  getIrRemote();
  
  //setRele();
  if(ldrValor > 900)
    setUsb(false);
  else
    setUsb(true);

  check_serial();
  resetClick();

  showDisplay();
  delay(300);
}

void check_serial() {
  if (Serial1.available()) {
    String resp = "";
    while (Serial1.available())
    {
      // The esp has data so display its output to the serial window
      char c = Serial1.read(); // read the next character.
      resp += c;
    }
    //resp = Serial1.readString();
    Serial.flush();
    
    Serial.println(resp);
    if(resp.indexOf("m=") > -1){
      month = getValue(resp, '&', 0);
      hour = getValue(resp, '&', 1);
      day = getValue(resp, '&', 2);
    }
    if(resp.indexOf("d0") > -1){ 
      setOnOff(rele1, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d1") > -1 ){ 
      setOnOff(rele2, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d2") > -1 ){ 
      setOnOff(rele3, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d3") > -1 ){ 
      setOnOff(rele4, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d4") > -1 ){ 
      setOnOff(rele5, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d5") > -1 ){ 
      setOnOff(rele6, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d6") > -1 ){ 
      setOnOff(rele7, getValue(resp, '=', 1).toInt());
    }else if(resp.indexOf("d7") > -1 ){ 
      setOnOff(rele8, getValue(resp, '=', 1).toInt());
    }

  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void resetClick(){
    if (digitalRead(button) == HIGH) {
        Serial.println("click button");
        Serial1.write("reset");
    }
}

void showDisplay(){

  if( (millis() - millisLcd) > 15000 ){
    lcd.setCursor(16,1);
    lcd.scrollDisplayLeft();
  }

  if( (millis() - millisLcd) > 24000 ){
    showHour = !showHour;
    millisLcd = millis();
  }
  
  if( (millis() - millisLcd) < 15000 ){
  lcd.clear();
  lcd.setBacklight(HIGH);
  if(showHour){
    printHour(0);
    printTemp(distance);
  }else{
    printTemp(0);
    printHour(distance);
  }
  }
}

void printHour(int start){
    lcd.setCursor(start,0);
    lcd.print(getValue(hour, '=', 1));
    lcd.setCursor(start+7,0);
    if(day.indexOf("") > -1){
      lcd.print(daysOfTheWeek[getValue(day, '=', 1).toInt()]);
    }
    lcd.setCursor(start,1);
    lcd.print(getValue(month, '=', 1));
}

void printTemp(int start){
   lcd.setCursor(start,0);
   lcd.print("T");
   lcd.setCursor(start+2,0);
   lcd.print(t);
   lcd.setCursor(start+8,0);
   lcd.print("L");
   lcd.setCursor(start+10,0);
   lcd.print(ldrValor);
   lcd.setCursor(start,1);
   lcd.print("U");
   lcd.setCursor(start+2,1);
   lcd.print(h);
   lcd.setCursor(start+8,1);
   lcd.print("C");
   lcd.setCursor(start+10,1);
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
    } 
    irrecv.resume();
  }
}

void setOnOff(int pin, bool choice){
  if(!choice)
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

void setUsb(bool option){
  //Serial.println(option);
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
   emon1.calcVI(17, 100); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS / TEMPO LIMITE PARA FAZER A MEDIÇÃO)
   currentDraw = emon1.Irms; //VARIÁVEL RECEBE O VALOR DE CORRENTE RMS OBTIDO
   currentDraw = currentDraw-ruido; //VARIÁVEL RECEBE O VALOR RESULTANTE DA CORRENTE RMS MENOS O RUÍDO
  
   if(currentDraw < 0){ //SE O VALOR DA VARIÁVEL FOR MENOR QUE 0, FAZ 
      currentDraw = 0; //VARIÁVEL RECEBE 0
   }
//   Serial.print("Corrente medida: ");
//   Serial.print(currentDraw);
//   Serial.println("A");
     
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
