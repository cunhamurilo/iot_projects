//libraries
#include <MFRC522.h>
#include <SPI.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <WiFiManager.h>

#define RST_PIN   D1
#define SS_PIN    D2
#define PIR_PIN   D0
#define VERM_LED  D3
#define VERD_LED  D4
#define RELE1_PIN 3   //rx
#define RESET_PIN D8   
#define LUZ_PIN A0

#define FIREBASE_HOST "your firebase id credentials"
#define FIREBASE_AUTH "your firebase authorization"

FirebaseData data;

const long utcOffsetInSeconds = -10800;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

WiFiManager wifiManager;//Objeto de manipulação do wi-fi

unsigned long millisInvaded = 0, millisCard = 0, millisLed = 0;

int luz = 0, cont = 0;
bool presence = false, alarm = false, startedWiFi = false, cardCheck = false;

// network
const char* ssid = "";      // put your ssid with start connection
const char* password = "";  // put your password with start connection

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String path = "", user = "";
String id = String(ESP.getFlashChipId());
  
void setup() {
  delay(2000);
  
  Serial.begin(115200);   // Initiate a serial communication
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
//  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  Serial.println("config");
  
  pinMode(PIR_PIN, INPUT);
  pinMode(LUZ_PIN, INPUT);
  pinMode(RESET_PIN, INPUT);
  pinMode(VERM_LED, OUTPUT);
  pinMode(VERD_LED, OUTPUT);
  pinMode(RELE1_PIN, OUTPUT);
  
  digitalWrite(VERD_LED, LOW);
  digitalWrite(VERM_LED, LOW);
  digitalWrite(RELE1_PIN,HIGH);

  millisLed = millis();
  delay(2000);
}

void loop() {

    resetClick();
    
    luz = analogRead(LUZ_PIN);
    Serial.print("LUZ: ");
    Serial.println(luz);
        
    presence =  getPresence();
      
    if(luz < 200){
      Serial.println(presence);
      if(presence == 1){
         digitalWrite(RELE1_PIN,LOW);
      }else{
         digitalWrite(RELE1_PIN,HIGH);
         millisInvaded = 0;
      }
    }else{
       digitalWrite(RELE1_PIN,HIGH);
    }

    if(alarm){
      if(cont == 1){
        millisInvaded = millis();
      }
    }  

    if(millisInvaded > 0){
      if(millis() - millisInvaded > 10000){
          if(alarm){
              if(WiFi.status() == WL_CONNECTED){
                  Serial.println("Send firebase ");
                  String date = getDate();
                  Serial.print("Date: ");
                  Serial.println(date);

                  FirebaseJson json;                
                  json.set("date", date);
                  
                  if (Firebase.pushJSON(data, "your path to history", json)) {
                    Serial.println("Add json to firebase");
                  }
              }
          }
          millisInvaded = 0;
      }
    }
    delay(50);

    if(cardCheck){
      if(millis() - millisCard > 2000){
        millisCard = 0;
        cardCheck = false;
      }
    }else{
        getCardCheck();
    }

    if(WiFi.status() == WL_CONNECTED){
      digitalWrite(VERM_LED, LOW);
      if(alarm){
        if(millis() - millisLed < 300){
          digitalWrite(VERD_LED, HIGH);
        }else if(millis() - millisLed < 600){
          digitalWrite(VERD_LED, LOW);
        }else{
          millisLed = millis();
        }
      }else{
        digitalWrite(VERD_LED, HIGH);
      }
      
      if(!startedWiFi){
        Serial.print("Conectado wifi: ");
        Serial.println(WiFi.SSID());
        Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
    
        DynamicJsonDocument doc(1024);
        if(Firebase.get(data, "your unique id devices path")){
           deserializeJson(doc, data.jsonString());
           String aux = doc["user"];
           user = aux;
           String aux2 = doc["path"];
           path = aux2;
        }
        timeClient.begin();
        startedWiFi = true;
      }
    }else{
      if(WiFi.SSID() != ""){
        Serial.println("Conectando wifi");
        connectWiFi();
      }else{
        Serial.println("Sem rede");
        digitalWrite(VERD_LED, LOW);
        digitalWrite(VERM_LED, HIGH);
      }
      startedWiFi = false;
    }
}

boolean getPresence(){
    presence = digitalRead(PIR_PIN);
    if(presence == 1)
      cont++;
    else
      cont = 0;
    
    return presence; 
}

void getCardCheck(){
   
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
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
  if (content.substring(1) == "A7 A7 CE 2C" || 
      content.substring(1) == "D3 81 7A 1A" || 
      content.substring(1) == "F9 8F 7D 59" || 
      content.substring(1) == "60 C2 2D E3")
  {   // change UID of the card that you want to give access
    Serial.println(" Access Granted ");
    Serial.println(" Welcome Mr.Circuit ");
    Serial.println(" Have FUN ");
    Serial.println();

    alarm = !alarm;
  }else{
    Serial.println(" Access Denied ");
  }

  millisCard = millis();
  cardCheck = true;
}


void connectWiFi(){
    if(WiFi.status() != WL_CONNECTED){
       resetClick();
       if(millis() - millisLed < 300){
          digitalWrite(VERM_LED, HIGH);
       }else if(millis() - millisLed < 600){
          digitalWrite(VERM_LED, LOW);
       }else{
          millisLed = millis();
       }
       Serial.print(".");
    }
}

void resetClick(){
    if (digitalRead(RESET_PIN) == HIGH) {
      Serial.println("Abertura Portal"); //Abre o portal
      digitalWrite(VERM_LED,HIGH); //Acende LED Vermelho
      digitalWrite(VERD_LED,LOW);
      wifiManager.resetSettings();       //Apaga rede salva anteriormente
      delay(20);
      
      //callback para quando entra em modo de configuração AP
      wifiManager.setAPCallback(configModeCallback); 
      //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
      wifiManager.setSaveConfigCallback(saveConfigCallback); 
      
      if(!wifiManager.autoConnect("ESP_SYSTEM", "12345678") ){ //Nome da Rede e Senha gerada pela ESP
      
        Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
        delay(2000);
        wifiManager.resetSettings();  
        ESP.reset(); //Reinicia ESP após não conseguir conexão na rede
      
      }else{       //Se caso conectar 
        Serial.println("Conectado na Rede!!!");
        FirebaseJson json;                
        json.set("id", id);
        json.set("type", "SECURITYSYSTEM");

        if (Firebase.pushJSON(data, "your path of available devices", json)) {
          Serial.println("Add json to firebase");
        }

        delay(1000);
     
        ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
      }
   }
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

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}
 
//Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}
