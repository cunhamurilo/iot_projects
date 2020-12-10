#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <time.h>

#define FIREBASE_HOST "your firebase id"
#define FIREBASE_AUTH "your firebase auth database"

const char *ssid     = "your ssid";
const char *password = "your password";

String user = "", path0 = "", path1 = "", path2 = "", path3 = "", path4 = "", path5 = "", path6 = "", path7 = "";
bool onOff1 = false, onOff2 = false, onOff3 = false, onOff4 = false, onOff5 = false, onOff6 = false, onOff7 = false, onOff8 = false;
bool onOffCheck = false;

const long utcOffsetInSeconds = -10800;
int timeConnect = 5000;

unsigned long millisSeg = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org", utcOffsetInSeconds);

String id = String(ESP.getFlashChipId());

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  connectWiFi();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
  FirebaseObject object = Firebase.get("devices-path/"+id);
  user = object.getString("user");
  path0 = object.getString("path0");
  path1 = object.getString("path1");
  path2 = object.getString("path2");
  path3 = object.getString("path3");
  path4 = object.getString("path4");
  path5 = object.getString("path5");
  path6 = object.getString("path6");
  path7 = object.getString("path7");
  
  timeClient.begin();
  millisSeg = 60001;
}

void loop() {

  if(WiFi.status() == WL_CONNECTED){

    if( millis() - millisSeg > 60000){
      String serial = "";
      serial = getDate();
      const char* serialChar = serial.c_str();
      Serial.write(serialChar);
      millisSeg = millis();
    }
    
    onOff1 = getOnOff(onOff1, path0, "d0=");
    onOff2 = getOnOff(onOff2, path1, "d1=");
    onOff3 = getOnOff(onOff3, path2, "d2=");
    onOff4 = getOnOff(onOff4, path3, "d3=");
    onOff5 = getOnOff(onOff5, path4, "d4=");
    onOff6 = getOnOff(onOff6, path5, "d5=");
    onOff7 = getOnOff(onOff7, path6, "d6=");
    onOff8 = getOnOff(onOff8, path7, "d7=");

  }else{
    connectWiFi();
  }

}

void connectWiFi(){
  int cont = 0;
  Serial.println("Conectando");
  while ( WiFi.status() != WL_CONNECTED && cont < timeConnect) {
    delay ( 500 );
    Serial.print ( "." );
    cont++;
  }
  Serial.println("Conectado ao wifi");
  
}

bool getOnOff(bool oldOnOff, String pathOnOff, String device){

    onOffCheck = Firebase.getBool("your path in firebase database device");
    if(oldOnOff != onOffCheck){
      String serial = device+String(onOffCheck);
      const char* serialChar = serial.c_str();
      Serial.write(serialChar);
      return onOffCheck;
    }else{
      return oldOnOff;
    }
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
