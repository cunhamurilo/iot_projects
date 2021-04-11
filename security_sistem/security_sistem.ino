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

#define FIREBASE_HOST "sparky-18bea.firebaseio.com" //your firebase id credentials"
#define FIREBASE_AUTH "Mt2X1Qhsp62JMRV1ZUr30JrP5mcmkY5UFdPdeypo" //your firebase authorization"

FirebaseData data;

const long utcOffsetInSeconds = -10800;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

WiFiManager wifiManager;//Objeto de manipulação do wi-fi

unsigned long millisInvaded = 0, millisCard = 0, millisLed = 0, millisRele = 0, millisPir = 0, millisSetAlarm = 0;

int luz = 0, cont = 0;
bool alarm = false, startedWiFi = false, cardCheck = false, startedOn = false;
int currentValue = 0, lastValue = -1; 

// network
const char* ssid = "";      // put your ssid with start connection
const char* password = "";  // put your password with start connection

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String path = "", user = "";
String id = String(ESP.getFlashChipId());
  
void setup() {
  delay(2000);
  
  Serial.begin(115200);   // Inicializa comunicação serial 
  
  SPI.begin();          // Inicializa  SPI bus
  mfrc522.PCD_Init();   // Inicializa  MFRC522
  //  mfrc522.PCD_DumpVersionToSerial();  // Mostra detalhes do MFRC522 Card Reader

  Serial.println("config");

  // defini I/O dos botoes  e leds
  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(LUZ_PIN, INPUT);
  pinMode(RESET_PIN, INPUT);
  pinMode(VERM_LED, OUTPUT);
  pinMode(VERD_LED, OUTPUT);
  pinMode(RELE1_PIN, OUTPUT);
  
  digitalWrite(VERD_LED, LOW);
  digitalWrite(VERM_LED, LOW);
  digitalWrite(RELE1_PIN,HIGH);

  millisLed = millis();

  // verifica se o sensor de presença inicia ativado
  currentValue = digitalRead(PIR_PIN);
  Serial.print("currentValue started: ");
  Serial.println(currentValue);

  if(currentValue == 1){
    startedOn = true;
  }
  delay(2000);
}

void loop() {

    // obtem a quantidade de luz
    luz = analogRead(LUZ_PIN);

    // verifica se o botão de reset de wifi foi ativado
    resetClick();

    // obtem se existe presença ou não
    getPresence();

    // se a quantidade de luz for baixa ativa rele para acender lampada
    if(luz < 300){
//      Serial.print("LUZ: ");
//      Serial.println(luz);
//      Serial.print("cont: ");
//      Serial.println(cont);

      // ativa lampada
      if(cont == 1 && millisRele == 0){
         digitalWrite(RELE1_PIN,LOW);
         Serial.println("RELE ON");
         millisRele = millis();
      }
    }

    // verifica se passou 15 segundos da lampada acesa
    if(millisRele > 0){
      if(millis() - millisRele > 15000){
         Serial.println("RELE OFF");
         digitalWrite(RELE1_PIN,HIGH);
         millisRele = 0;
      }
    }

    // espera 20 segundos para verificar presenca
    if(millisSetAlarm > 0){
      if(millis() - millisSetAlarm > 20000){
        // verifica se o alarme está ativado e se existe presenca
        if(alarm){
          if(cont == 1){
            millisInvaded = millis();
          }
        }  
        
      }
    }
    
    // verifica se passou 10 segundos e não foi passado o cartão para desativar o alarme
    if(millisInvaded > 0){
      if(millis() - millisInvaded > 10000){
          // se não desativou o alarme envia uma notificação para o firebase com o dia e a hora
          if(alarm){
              if(WiFi.status() == WL_CONNECTED){
                  Serial.println("Send firebase ");
                  String date = getDate();
                  Serial.print("Date: ");
                  Serial.println(date);

                  FirebaseJson json;                
                  json.set("date", date);

                  String pathFirebase = "users/"+user+"/devices/"+path+"/history/";//"your path to history";
                  
                  if (Firebase.pushJSON(data, pathFirebase, json)) {
                    Serial.println("Add json to firebase");
                  }
              }
          }
          millisInvaded = 0;
      }
    }

    // verifica se está conectado na internet
    if(WiFi.status() == WL_CONNECTED){
      digitalWrite(VERM_LED, LOW);

      // se alarme estiver ativado a led verde fica piscando
      if(alarm){
        if(millisSetAlarm > 0){
          if(millis() - millisSetAlarm > 20000){
            if(millis() - millisLed < 300){
              digitalWrite(VERD_LED, HIGH);
            }else if(millis() - millisLed < 600){
              digitalWrite(VERD_LED, LOW);
            }else{
              millisLed = millis();
            }
          }
        }
      }else{
        digitalWrite(VERD_LED, HIGH);
      }

      // simula a função setup para comunicação do firebase e ntc
      if(!startedWiFi){
        Serial.print("Conectado wifi: ");
        Serial.println(WiFi.SSID());
        Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
    
        DynamicJsonDocument doc(1024);
        String pathFirebase = "devices-path/"+id;//"your unique id devices path";
        if(Firebase.get(data, pathFirebase)){
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
    
    delay(10);

    // verifica se as tag RDIF foram inseridas
    if(cardCheck){
      if(millis() - millisCard > 2000){
        millisCard = 0;
        cardCheck = false;
      }
    }else{
        getCardCheck();
    }
}

// função que verifica a presença
void getPresence(){

    // le dados do sensor
    currentValue = digitalRead(PIR_PIN);
    //Serial.println(currentValue);

    // verifica mudança da variável
    if(lastValue != currentValue){
         // sem presença
        if(currentValue == 0)
            cont = 0;

        // vefica se não estartou on e se o valor da presença é on
        if(lastValue == 0 && currentValue == 1 && startedOn == true)
            startedOn = false;
            
        // substitui o valor antigo da variavel caso diferente
        lastValue = currentValue;
    }

    // presenca e não estartou on
    if(currentValue == 1 && startedOn == false)
        cont++;

    delay(10);
}

// função que verifica a presença de tag RDIF
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

    if(alarm)
      millisSetAlarm = millis();
    else
      millisSetAlarm = 0;
      
  }else{
    Serial.println(" Access Denied ");
  }

  millisCard = millis();
  cardCheck = true;
}

// função que verifica WIFI
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

// função de click do botao de reset do wifi
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

        if (Firebase.pushJSON(data, "available-devices/", json)) {
          Serial.println("Add json to firebase");
        }

        delay(1000);
     
        ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
      }
   }
}

// função que obtem os dados de dia e hora
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
