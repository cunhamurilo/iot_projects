#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

Servo lockServo;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static int pinA = 2;
static int pinB = 3;
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte prevEncoderPos = 0;
volatile byte reading = 0;

const byte buttonPin = 4;
byte oldButtonState = HIGH;
const unsigned long debounceTime = 10;
unsigned long buttonPressTime;

byte code[4] = {0,0,0,0};
byte codeGuess[4] = {0,0,0,0};
byte timer[4] = {0,0};
byte Digit = 0;
byte DigitGuess = 0;
byte DigitTimer = 0;
int Count = 0;
//-------------------------------------------------------//
void setup()
{
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  lockServo.attach(5);
  lockServo.write(140);
  
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(0,PinA,RISING);
  attachInterrupt(1,PinB,RISING);
  pinMode (buttonPin, INPUT_PULLUP);

  Digit = 0;
  DigitGuess = 0;
  DigitTimer = 0;
  Count = 0;
  
  CriarSenha();
  InserirTempo();
  FecharCofre();
}
//-------------------------------------------------------//
void loop() 
{
  Regressiva();
}                                                                                                                                                                                                                                                                                                 
//-------------------------------------------------------//
void CriarSenha()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(35,10);
  display.println(F("Cofre"));
  display.display(); 
  delay(1000);
  display.clearDisplay();
  display.setCursor(45,10);
  display.println(F("com"));
  display.display(); 
  delay(1000);
  display.clearDisplay();
  display.setCursor(35,10);
  display.println(F("Timer")); 
  display.display(); 
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20,10);
  display.print(F("Aperte o botao"));
  display.setCursor(10,20);
  display.print(F("para criar a senha"));
  display.display();
  
  boolean started = false;
  boolean pressed = false;
  while(!started)
  {
    byte buttonState = digitalRead (buttonPin); 
    if (buttonState != oldButtonState)
    {
      if (millis () - buttonPressTime >= debounceTime)
      {
        buttonPressTime = millis();
        oldButtonState =  buttonState;
        if (buttonState == LOW)
        {
          pressed = true;
        }
        else 
        {
          if (pressed == true)
          {
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(40,10);
            display.print(F("0000"));
            display.display();
            started = true;
          }
        }  
      }
    }
  }
  for(int i=0 ; i<=3 ; i++)
  {
    Digit = i;
    boolean confirmed = false;
    boolean pressed = false;
    encoderPos = 0; 
    while(!confirmed)
    {
      byte buttonState = digitalRead (buttonPin); 
      if (buttonState != oldButtonState)
      {
        if (millis () - buttonPressTime >= debounceTime)
        {
          buttonPressTime = millis ();
          oldButtonState =  buttonState;
          if (buttonState == LOW)
          {
            code[i] = encoderPos;
            pressed = true;
          }
          else 
          {
            if (pressed == true)
            {
              SenhaDisplay();
              confirmed = true;
            }
          }  
        }
      }
      if(encoderPos!=prevEncoderPos)
      {
        SenhaDisplay();
        prevEncoderPos=encoderPos;
      }
    }
  }
  encoderPos = 0;
}
//-------------------------------------------------------//
void InserirTempo()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(5,10);
  display.print(F("Aperte o botao para"));
  display.setCursor(17,20);
  display.print(F("inserir o tempo"));
  display.display();
  
  boolean started = false;
  boolean pressed = false;
  while(!started)
  {
    byte buttonState = digitalRead (buttonPin); 
    if (buttonState != oldButtonState)
    {
      if (millis () - buttonPressTime >= debounceTime)
      {
        buttonPressTime = millis();
        oldButtonState =  buttonState;
        if (buttonState == LOW)
        {
          pressed = true;
        }
        else 
        {
          if (pressed == true)
          {
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(50,10);
            display.print(F("00"));
            display.display();
            started = true;
          }
        }  
      }
    }
  }
  
  for(int i=0 ; i<=1 ; i++)
  {
    DigitTimer = i;
    boolean confirmed = false;
    boolean pressed = false;
    encoderPos = 0; 
    while(!confirmed)
    {
      byte buttonState = digitalRead (buttonPin); 
      if (buttonState != oldButtonState)
      {
        if (millis () - buttonPressTime >= debounceTime)
        {
          buttonPressTime = millis ();
          oldButtonState =  buttonState;
          if (buttonState == LOW)
          {
            timer[i] = encoderPos;
            pressed = true;
          }
          else 
          {
            if (pressed == true)
            {
              TimerDisplay();
              confirmed = true;
            }
          }  
        }
      }
      if(encoderPos!=prevEncoderPos)
      {
        TimerDisplay();
        prevEncoderPos=encoderPos;
      }
    }
  }
  encoderPos = 0;
}
//-------------------------------------------------------//
void SenhaDisplay()
{
  String pass = "";
  
  for (int i=0 ; i<Digit ; i++)
  {
    pass = pass + code[i];
  }
  pass = pass + encoderPos;
  for (int i=Digit+1 ; i<=3 ; i++)
  {
    pass = pass + "0";
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(40,10);
  display.println(pass);
  display.display(); 
}
//-------------------------------------------------------//
void TimerDisplay()
{
  String temp = "";
  
  for (int i=0 ; i<DigitTimer ; i++)
  {
    temp = temp + timer[i];
  }
  temp = temp + encoderPos;
  for (int i=DigitTimer+1 ; i<=1 ; i++)
  {
    temp = temp + "0";
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(50,10);
  display.println(temp);
  display.display();
}
//-------------------------------------------------------//
void FecharCofre()
{
 display.clearDisplay();
 display.setTextSize(1);
 display.setCursor(25,10);
 display.print(F("Programado!"));
 display.setCursor(15,20);
 display.print(F("Fechando cofre..."));
 display.display();
 delay(5000);
 lockServo.write(45);
 delay(500);
 display.clearDisplay();
 display.setCursor(0,0);
 display.print(F("Sua senha:"));
 display.setTextSize(2);
 display.setCursor(40,10);
 for(int i=0 ; i<=3 ; i++)
 {
  display.print(code[i]);
 }
 display.display();
 delay(2000);
 display.clearDisplay();
 display.setTextSize(1);
 display.setCursor(0,10);
 display.print(F("O cofre vai abrir em"));
 display.setCursor(30,20);
 for(int i=0 ; i<=1 ; i++)
 {
  display.print(timer[i]);
 }
 display.print(F(" minuto(s)"));
 display.display();
 delay(2000); 
}
//-------------------------------------------------------//
void Regressiva()
{
  String score = "";
  for(int i=0 ; i<=1 ; i++)
  {
    score = score + timer[i];
  }
  Count = score.toInt();
  for (Count; Count>=1; Count--)
  { 
    if (Count < 10)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(F("0"));
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();
      display.clearDisplay();
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(F("0"));
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();
      display.clearDisplay();
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(F("0"));
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();  
    }
    else
    {
      display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();
      display.clearDisplay();
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();
      display.clearDisplay();
      display.setCursor(33,10);
      display.print(F("Aguarde"));
      display.setCursor(20,20);
      display.print(Count);
      display.println(F(" minuto(s)"));
      display.display(); 
      delay(15000);
      display.clearDisplay();
      display.setCursor(10,10);
      display.print(F("Ou segure o botao"));
      display.setCursor(5,20);
      display.print(F("para inserir a senha"));
      display.display();
      InserirSenha();
    } 
    if (Count == 1)
    {
     display.clearDisplay();
     display.setTextSize(1);
     display.setCursor(10,10);
     display.print(F("Tempo finalizado!"));
     display.setCursor(15,20);
     display.print(F("Abrindo cofre..."));
     display.display();
     delay(2000);
     lockServo.write(140);
     setup();  
    }
  }
} 
//-------------------------------------------------------//
void InserirSenha()
{     
  boolean pressed = false;
  int dec = 0;
  for (dec; dec<=5; dec++)
  { 
    byte buttonState = digitalRead (buttonPin); 
    if (buttonState != oldButtonState)
    {
      if (millis () - buttonPressTime >= debounceTime)
      {
        buttonPressTime = millis();
        oldButtonState =  buttonState;
        if (buttonState == LOW)
        {
          pressed = true;
        }
        if (pressed == true)
        {
          TentarSenha();
        }
      }
    }  
    delay(1000);
  }      
}
//-------------------------------------------------------//
void TentarSenha()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(40,10);
  display.print(F("0000"));
  display.display();
  
  for(int i=0 ; i<=3 ; i++)
  {
    DigitGuess = i;
    boolean confirmed = false;
    boolean pressed = false;
    encoderPos = 0; 
    while(!confirmed)
    {
      byte buttonState = digitalRead (buttonPin); 
      if (buttonState != oldButtonState)
      {
        if (millis () - buttonPressTime >= debounceTime)
        {
          buttonPressTime = millis ();
          oldButtonState =  buttonState;
          if (buttonState == LOW)
          {
            codeGuess[i] = encoderPos;
            pressed = true;
          }
          else 
          {
            if (pressed == true)
            {
              TentarSenhaDisplay();
              confirmed = true;
            }
          }  
        }
      }
      if(encoderPos!=prevEncoderPos)
      {
        TentarSenhaDisplay();
        prevEncoderPos=encoderPos;
      }
    }
  }
  encoderPos = 0;
  VerificarSenha();
}
//-------------------------------------------------------//
void TentarSenhaDisplay()
{
  String tryPass = "";
  for (int i=0 ; i<DigitGuess ; i++)
  {
    tryPass = tryPass + codeGuess[i];
  }
  tryPass = tryPass + encoderPos;
  for (int i=DigitGuess+1 ; i<=3 ; i++)
  {
    tryPass = tryPass + "0";
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(40,10);
  display.println(tryPass);
  display.display(); 
}
//-------------------------------------------------------//
void VerificarSenha()
{
  int correctDigit = 0;
  boolean correctGuess =  false;
  for (int i=0 ; i<= 3 ; i++)
  {
    if (code[i]==codeGuess[i])
    {                                                                      
      correctDigit++;
    }
    if (correctDigit==4)
    {
      correctGuess = true;
    }
    else
    {
      correctGuess = false;
    }
  }
  if (correctGuess == true)
  {
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(20,10);
   display.print(F("Senha correta!"));
   display.setCursor(15,20);
   display.print(F("Abrindo cofre..."));
   display.display();
   delay(2000);
   lockServo.write(140);
   setup();  
  }
  else
  {
   display.clearDisplay();
   display.setTextSize(1);
   display.setCursor(15,15);
   display.print(F("Senha incorreta!"));
   display.display();
   delay(2000);
  }
}
//-------------------------------------------------------//  
void PinA()
{
  cli();
  reading = PIND & 0xC;
  if(reading == B00001100 && aFlag)
  {     
    if(encoderPos>0)
      encoderPos --;
    else
      encoderPos = 9;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00000100)
    bFlag = 1;
  sei();
}
//-------------------------------------------------------//
void PinB()
{
  cli();
  reading = PIND & 0xC;
  if (reading == B00001100 && bFlag)
  {
    if(encoderPos<9)
      encoderPos ++;
    else
      encoderPos = 0;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00001000)
    aFlag = 1;
  sei();
}
