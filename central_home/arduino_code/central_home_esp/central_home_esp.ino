#include <NTPClient.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <time.h>
#include <EEPROM.h>
#include <FS.h>
#include <WiFiManager.h>

WiFiManager wifiManager;//Objeto de manipulação do wi-fi

#define FIREBASE_HOST "your firebase id"
#define FIREBASE_AUTH "your firebase auth database"

FirebaseData data;
FirebaseData data2;

const char *ssid     = ""; // your ssid if start connection
const char *password = "";// your password if start connection

const long utcOffsetInSeconds = -10800;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

String id = String(ESP.getFlashChipId());
String serial = "", oldSerial = "";

String parentPath = "";
String childPath[8] = {"","","","","","","",""};
size_t childPathSize = 8;

String incomingByte = "";
      
void writeFile(String state, String path) { 
  //Abre o arquivo para escrita ("w" write)
  //Sobreescreve o conteúdo do arquivo
  File rFile = SPIFFS.open(path,"w+"); 
  if(!rFile){
    Serial.println("Erro ao abrir arquivo!");
  } else {
    rFile.println(state);
//    Serial.println("Gravou arquivo");
//    Serial.println(state);
  }
  rFile.close();
}

String readFile(String path) {
  File rFile = SPIFFS.open(path,"r");
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  String content = rFile.readStringUntil('\r'); //desconsidera '\r\n'
//  Serial.print("leitura do arquivo: ");
//  Serial.println(content);
  rFile.close();
  return content;
}

void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    Serial.println("\nErro ao abrir o sistema de arquivos");
  } else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void streamCallback(MultiPathStreamData stream){
  Serial.println();
  Serial.println("Stream Data1 available...");

  size_t numChild = sizeof(childPath)/sizeof(childPath[0]);

  for(size_t i = 0; i < numChild; i++){
    if (stream.get(childPath[i])){
//      /Serial.println("path: " + stream.dataPath + ", type: " + stream.type + ", value: " + stream.value);
      
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, stream.value);
      
      boolean sensor = false;
      if(stream.dataPath.indexOf("traits") == -1){
         sensor = doc["traits"]["OnOff"]["on"];
      }else{
         if(stream.type.indexOf("boolean") == -1){
            sensor = doc["on"];
         }else{
            if(stream.value == "true"){
              sensor = true;
            }
         }
      }
      
      setEventUpdate(sensor, "path"+String(i));
      writeFile(String(sensor), "/rele"+String(i)+".txt");
    }
  }

  Serial.println();
}

void streamTimeoutCallback(bool timeout){
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}

void setup(){
  Serial.begin(115200);

  data.setBSSLBufferSize(4096, 4096);
  data.setResponseSize(1024);
         
  data2.setBSSLBufferSize(1024, 1024);
  data2.setResponseSize(1024);

  // Inicializa a EEPROM para a versão 
  EEPROM.begin(4);
  delay(1000);
  
  if (WiFi.SSID() != "") {
    Serial.println(WiFi.SSID());
    connectWiFi();
    openFS();
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true); 

    uint8_t readPath = EEPROM.read(0);
    Serial.println(readPath );
    DynamicJsonDocument doc(1024);
    if(readPath == 1){
      Serial.println("get data from firebase");
      if(Firebase.get(data2, "/devices-path/"+id)){
         deserializeJson(doc, data2.jsonString());
         
         String user = doc["user"];
         
         String input = "";
         input = "{\"user\":\""+user+"\",";
         
         bool onOff = false;
  
         for(int i = 0; i < 8; i++){
            String path = doc["path"+String(i)];
            if(i < 7)
              input += "\"path"+String(i)+"\":\""+path+"\",";
            else
              input += "\"path"+String(i)+"\":\""+path+"\"}";
    
            if(Firebase.getBool(data2, "/users/"+user+"/devices/"+path+"/traits/OnOff/on")){
              onOff = data2.boolData();
              writeFile(String(onOff), "/rele"+String(i)+".txt");
            }
         }
         
         writeFile(input, "/state.txt");
         
         EEPROM.write(0, 0);
         EEPROM.commit();
         delay(3000);
         ESP.restart();
      }
    }
    String state = readFile("/state.txt"); 
    deserializeJson(doc, state);
  
    String user = doc["user"];
    parentPath = "/users/"+user+"/devices";
    
    delay(4000);
      
    for(int i = 0; i < 8; i++){
       String onOff = readFile("/rele"+String(i)+".txt");
       if(onOff.compareTo("1") == 0)
         setEventUpdate(true, "path"+String(i));
       else
         setEventUpdate(false, "path"+String(i));
      delay(1000);
      String p = doc["path"+String(i)];
      childPath[i] = "/"+p;
    }
           
    if (!Firebase.beginMultiPathStream(data, parentPath, childPath, childPathSize))
    {
      Serial.println("------------------------------------");
      Serial.println("Can't begin stream connection...");
      Serial.println("REASON: " + data.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  
    Firebase.setMultiPathStreamCallback(data, streamCallback, streamTimeoutCallback);
  
    delay(1000);
    
    timeClient.begin();
  }
}

void loop() {
    resetClick();
  
    // get string formated with time and hour
    if(WiFi.status() == WL_CONNECTED){
      serial = getDate();
    }
    //check if passed time 1min
    if ( serial.compareTo(oldSerial) != 0 ){
      oldSerial = serial;
      const char* serialChar = serial.c_str();
      Serial.write(serialChar);
    }

    // check if available firebase data
    if(WiFi.status() == WL_CONNECTED){

    }else{
      if(WiFi.SSID() != ""){
        connectWiFi();
      }
    }

}

void connectWiFi(){
  Serial.println("Conectando");
  while ( WiFi.status() != WL_CONNECTED ) {
    resetClick();
    delay ( 400 );
    Serial.print ( "." );
  }
  Serial.println("Conectado ao wifi");
  
}

void setEventUpdate(bool data, String path){
//   Serial.print("data: ");
//   Serial.println(data);
   String device = "";
   if(path.compareTo("path0") == 0 ){
       device = "d0=";
   }else if(path.compareTo("path1") == 0 ){
       device = "d1=";
   }else if(path.compareTo("path2") == 0 ){
       device = "d2=";
   }else if(path.compareTo("path3") == 0 ){
       device = "d3=";
   }else if(path.compareTo("path4") == 0 ){
       device = "d4=";
   }else if(path.compareTo("path5") == 0 ){
       device = "d5=";
   }else if(path.compareTo("path6") == 0 ){
       device = "d6=";
   }else if(path.compareTo("path7") == 0 ){
       device = "d7=";
   }
   String serialFirebase = device+String(data);
   const char* serialChar = serialFirebase.c_str();
   Serial.write(serialChar);
}

String getDate(){
  timeClient.update();
  String date = "";

  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime(&rawtime);
           
  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);
  date = "m=" + dayStr + "/";
  
  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);
  date += monthStr + "/";
  
  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);
  date += yearStr;

  String h = timeClient.getHours() < 10 ? "0" + String(timeClient.getHours()) : String(timeClient.getHours());
  String m = timeClient.getMinutes() < 10 ? "0" + String(timeClient.getMinutes()) : String(timeClient.getMinutes());
  
  date += "&h=" + h + ":" + m;

  date += "&d="+ String(timeClient.getDay());
  
  return date;
}

void resetClick(){
    if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = "";
      while (Serial.available()){
        char c = Serial.read();
        incomingByte += c;
      }
      Serial.println(incomingByte);
      
      if (incomingByte.length() > 0) {
        if(incomingByte.compareTo("reset") == 0){
          Serial.println("Abertura Portal"); //Abre o portal
          wifiManager.resetSettings();       //Apaga rede salva anteriormente
          delay(20);
            
          //callback para quando entra em modo de configuração AP
          wifiManager.setAPCallback(configModeCallback); 
          //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
          wifiManager.setSaveConfigCallback(saveConfigCallback); 
            
          if(!wifiManager.autoConnect("ESP_HOME", "12345678") ){ //Nome da Rede e Senha gerada pela ESP
            
            Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
            delay(2000);
            wifiManager.resetSettings();  
            ESP.reset(); //Reinicia ESP após não conseguir conexão na rede
            
          }else{       //Se caso conectar 
            Serial.println("Conectado na Rede!!!");
            FirebaseJson json;                
            json.set("id", id);
            json.set("type", "HOMECENTRAL");
      
            if (Firebase.pushJSON(data2, "/available-devices", json)) {
               Serial.println("Add json to firebase");
            }else{
              Serial.println("Erro add json to firebase");
            }
      
            EEPROM.write(0, 1);
            EEPROM.commit();
            delay(1000);
           
            ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
          }  
        }
          
      }
    }
    
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
