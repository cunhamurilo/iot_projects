//libraries
#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define RST_PIN 5 //D1
#define SS_PIN 4 //D2
#define PIR_PIN 16 //D0
#define VERM_LED 2 //D4
#define VERD_LED 0 //D3
#define RELE1_PIN 3 //rx
//#define RELE2_PIN 1 //tx
#define LUZ_PIN A0

#define FIREBASE_HOST "your firebase id credentials"
#define FIREBASE_AUTH "your firebase authorization"

const long utcOffsetInSeconds = -10800;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

unsigned long millisSeg = 0;
unsigned long millisCard = 0;
unsigned long millisLed = 0;
unsigned long millisInvaded = 0;
float value = 0;
int luz = 0, countAway = 0, countHome = 0, count = 0, choice = 0, choice2 = 0, out = 0;
int timeConnect = 1000;
bool statuss = false;

const char* ssid = "your ssid";
const char* password = "your password"; 

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String path = "", user = "";
String id = String(ESP.getFlashChipId());
  
void setup() {
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  // Inicializa a EEPROM para a versão 
  EEPROM.begin(4);
  
  pinMode(PIR_PIN, INPUT);
  pinMode(LUZ_PIN, INPUT);
  pinMode(VERM_LED, OUTPUT);
  pinMode(VERD_LED, OUTPUT);
  pinMode(RELE1_PIN, OUTPUT);
  //pinMode(RELE2_PIN, OUTPUT);
  
  digitalWrite(VERD_LED, LOW);
  digitalWrite(VERM_LED, LOW);
  digitalWrite(RELE1_PIN,HIGH);
  
  WiFi.begin(ssid, password);
 
  Serial.println("Conectando");
  int cont = 0;
  cont = connectWifi(timeConnect);
  if(cont > timeConnect -1){
     //wifiManager.resetSettings();
  }else{
     Serial.println("conectado.");
     Serial.println(WiFi.localIP());
       
     Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 

     FirebaseObject object = Firebase.get("devices-path/"+id);
     user = object.getString("user");
     path = object.getString("path");
  
     timeClient.begin();
     millisCard = millis();
  }
  countAway = EEPROM.read(0);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    if(out == 0)
      digitalWrite(VERD_LED, LOW);
    else
      digitalWrite(VERD_LED, HIGH);
    digitalWrite(VERM_LED, LOW);
  }
    
  if((millis() - millisCard) > 4000){
    if(out == 1){
       if(statuss == false)
          value = digitalRead(PIR_PIN);
       //Serial.println(value);
       if(value == HIGH && statuss == false){
           Serial.println("alguem entrou e nao desativou o alarme");
           statuss = true;
           millisSeg = millis();
           countAway++;
           count++;  
           EEPROM.write(0, countAway);
           EEPROM.commit();
           
           Serial.print("Count Away: ");
           Serial.println(countAway);
           String date = "";
           if(WiFi.status() != WL_CONNECTED){
              Serial.println("Desconectado");
              digitalWrite(VERD_LED, LOW);
              connectWifi(timeConnect);
              Serial.println("Conectado a rede sem fio ");
           }
           if(WiFi.status() == WL_CONNECTED){
              date = getDate();
              StaticJsonBuffer<200> jsonBuffer;
              // create an object
              JsonObject& obj = jsonBuffer.createObject();
              obj["date"] = date;
              Firebase.push("path history", obj);
              Firebase.setInt("path device in firebase", countAway);
           
              Serial.println(date);
           }
           millisInvaded = millis();
       }
       if((millis() - millisSeg) > 5000 && statuss == true){
          statuss = false;
       }
       
       if(count > 0){
           millisInvaded = checkCardLed(choice, choice2, millisInvaded, 700);
       }
    }else{
        luz = analogRead(LUZ_PIN);
        //Serial.print("LUZ: ");
        //Serial.println(luz);
        
        if(luz > 1000){
          if(statuss == false)
            value = digitalRead(PIR_PIN);
          if(value == HIGH && statuss == false){
            Serial.println("Presença");
            statuss = true;
            millisSeg = millis();
            digitalWrite(RELE1_PIN,LOW);
            countHome++;
            
            Serial.print("Count Home: ");
            Serial.println(countHome);
          }
        }else if(luz < 1000 && statuss == false){
            millisSeg = 0;
            digitalWrite(RELE1_PIN,HIGH);
        }
        if( (millis() - millisSeg) > 10000 && statuss == true){
            Serial.println("10 segundos");
            statuss = false;
            digitalWrite(RELE1_PIN,HIGH);
        }
    }
  delay(50);
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == "your card uid"){// change UID of the card that you want to give access
    Serial.println(" Access Granted ");
    Serial.println(" Welcome Mr.Circuit ");
    Serial.println(" Have FUN ");
    Serial.println();
    if(out == 1){
      out = 0;
    }else{
      out = 1;
    } 
    choice = VERD_LED;
    choice2 = VERM_LED;
  }else   {
    Serial.println(" Access Denied ");
    choice = VERM_LED;
    choice2 = VERD_LED;
  }
  millisSeg = 0;
  millisCard = millis();
  millisLed = millis();
  digitalWrite(RELE1_PIN, HIGH);
  statuss = false;
  count = 0;
  }else{
    millisLed = checkCardLed(choice, choice2, millisLed, 200);
  }
} 

unsigned long checkCardLed(int ledChoice, int ledOff, unsigned long millisChoice, int timeMillis){
    digitalWrite(ledOff, LOW);
    if((millis() - millisChoice) > timeMillis){
       digitalWrite(ledChoice, LOW);
    }else{
       digitalWrite(ledChoice, HIGH);
    }
    if((millis() - millisChoice) > timeMillis * 2){
       millisChoice = millis();
    }
    return millisChoice;
}

int connectWifi(int timeConnect){
    int cont = 0;
    Serial.println("Conectando wifi");
    while(WiFi.status() != WL_CONNECTED && cont < timeConnect){
       digitalWrite(VERM_LED,HIGH);
       delay(200);
       digitalWrite(VERM_LED,LOW);
       delay(200);
       Serial.print(".");
       cont++;
    }
    if(cont > timeConnect - 1)
      return cont;
    else
      return 0;
}

String getDate(){
  timeClient.update();
  String date = "";
  date = daysOfTheWeek[timeClient.getDay()];

  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
           
  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);
  date += ", " + dayStr + "/";
  
  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);
  date += monthStr + "/";
  
  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);
  date += yearStr + ", ";
  
  String hour = timeClient.getFormattedTime();
  date += hour;

  return date;
}
