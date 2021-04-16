//Bibliotecas
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <WiFiManager.h>
#include "OpenWeatherMapOneCall.h"

// Pinos
#define TFT_CS         0
#define TFT_RST        2
#define TFT_DC        15
#define TFT_MOSI      13
#define TFT_SCLK      14
#define DHT_PIN       12  
#define PINO_RESET       16  

//Definições WiFi
const char* WIFI_SSID     = ""; // put your ssid if start connect
const char* WIFI_PASSWORD = ""; // put your ssid if start connect
WiFiClient wifiClient;

// Definições Cliente NTP
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = -10800;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiManager wifiManager;//Objeto de manipulação do wi-fi
 
//Definições Open Weather Map 
String OPEN_WEATHER_MAP_APP_ID = "your id";
//Go to https://www.latlong.net/ 
float OPEN_WEATHER_MAP_LOCATTION_LAT = -21.995077;
float OPEN_WEATHER_MAP_LOCATTION_LON = -47.896601;
String OPEN_WEATHER_MAP_LANGUAGE = "pt";
boolean IS_METRIC = true;

OpenWeatherMapOneCallData openWeatherMapOneCallData;

// Definições Dias da Semana e Mês
const String WDAY_NAMES[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
const String MONTH_NAMES[] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

// Definições Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Definições DHT11
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Definições BMP180
Adafruit_BMP085 bmp;

// Definição de cores
#define PRETO     0x0000
#define BRANCO    0xFFFF
#define CHUMBO    0x4208
#define TEMP1     0xCE39
#define TEMP2     0xEBF0
#define UMID1     0x7DDD
#define UMID2     0xCF1F
#define PRES1     0x7DDD
#define PRES2     0xCF1F
#define SOL1      0xFC29
#define SOL2      0xFE0C
#define SEN1      0xFE8E
#define SEN2      0xD6FD
#define NUB1      0x8451
#define NUB2      0xD6FD
#define CHU1      0x43B3
#define CHU2      0x7DDD
#define TEM1      0xB659
#define TEM2      0xFE08
#define TEM3      0xA65F

// Caracteres externos
extern unsigned char temperatura1[];
extern unsigned char temperatura2[];
extern unsigned char temperatura3[];
extern unsigned char umidade1[];
extern unsigned char umidade2[];
extern unsigned char umidade3[];
extern unsigned char pressao1[];
extern unsigned char pressao2[];
extern unsigned char pressao3[];
extern unsigned char sol1[];
extern unsigned char sol2[];
extern unsigned char sol3[];
extern unsigned char solenuvem1[];
extern unsigned char solenuvem2[];
extern unsigned char solenuvem3[];
extern unsigned char nublado1[];
extern unsigned char nublado2[];
extern unsigned char nublado3[];
extern unsigned char chuva1[];
extern unsigned char chuva2[];
extern unsigned char chuva3[];
extern unsigned char tempestade1[];
extern unsigned char tempestade2[];
extern unsigned char tempestade3[];
extern unsigned char tempestade4[];

double millisPrevisao = 0;

typedef struct Forecast{
  time_t observationTimestamp;
  int tmin;
  int tmax;
  String cond;
}Forecast;

Forecast forecast1;
Forecast forecast2;
Forecast forecast3;

void setup() {

  // Inicialização comunicação serial
  Serial.begin(115200);

  pinMode(PINO_RESET, INPUT);

  // Inicialização display
  tft.initR(INITR_BLACKTAB);
  
  if (WiFi.SSID() != "") {

    // Show display
    tft.fillScreen(CHUMBO);
    tft.setTextSize(1);
    tft.setTextColor(BRANCO);
    tft.setCursor(25,70);
    tft.print("Conectando...");
    
    // Inicialização WiFi
    connectWiFi();
    
    // Inicialização cliente NTP
    timeClient.begin();
    timeClient.update();
    
    delay(7000);
    
    // Inicialização DHT11
    dht.begin();
    
    // Inicialização BMP180
    bmp.begin();
    millisPrevisao = -25200000;
  }else{
    tft.fillScreen(CHUMBO);
    tft.setTextSize(1);
    tft.setTextColor(BRANCO);
    tft.setCursor(25,70);
    tft.print("Sem rede");
  }
}

void loop() {
  resetClick();
  
  if(WiFi.status() == WL_CONNECTED){
    resetClick();
    tft.fillScreen(CHUMBO);
    resetClick();
    data();
    resetClick();
    horario();
    resetClick();
    temperatura();
    resetClick();
    delay(10000);
    resetClick();
    
    resetClick();
    tft.fillScreen(CHUMBO);
    resetClick();
    data();
    resetClick();
    horario(); 
    resetClick();
    umidade();
    delay(10000);
    resetClick();
  
    resetClick();
    tft.fillScreen(CHUMBO);
    resetClick();
    data();
    resetClick();
    horario(); 
    resetClick();
    pressao();
    delay(10000);
    resetClick();
    
    resetClick();
    tft.fillScreen(CHUMBO);
    resetClick();
    data();
    resetClick();
    horario();
    resetClick();
    previsao();
    delay(10000);
    resetClick();
  }else{
     if(WiFi.SSID() != ""){
        connectWiFi();
     }
  }
}

void connectWiFi() {
  Serial.print("Conectando...");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    resetClick();
    delay(400);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado!");
  Serial.println(WiFi.localIP());
  Serial.println();
  delay(4000);
}

void data(){
  
  String weekDay = WDAY_NAMES[timeClient.getDay()];
  timeClient.update();
  
  tft.setCursor(15,2);
  tft.setTextSize(1);
  tft.setTextColor(BRANCO);
  tft.print(weekDay);
  
  tft.print(", ");
  
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm*ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  tft.print(monthDay);
  
  tft.print(" ");
  
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = MONTH_NAMES[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  tft.print(currentMonthName);
  
  tft.print(" ");
  
  tft.print(currentYear);
}

void horario(){
  
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  tft.setCursor(35,15);
  tft.setTextSize(2); 
  if (currentHour < 10){
    tft.print("0");
    tft.print(currentHour);
  }
  else {
    tft.print(currentHour); 
  } 
  tft.print(":");
  if (currentMinute < 10){
    tft.print("0");
    tft.print(currentMinute);
  }
  else {
    tft.print(currentMinute);
  } 
}

void temperatura(){

  tft.drawBitmap(16,32,temperatura1,96,96,TEMP1);
  tft.drawBitmap(16,32,temperatura2,96,96,TEMP2);
  tft.drawBitmap(16,32,temperatura3,96,96,PRETO);
  float t = bmp.readTemperature();
  tft.setCursor (30,130);
  tft.setTextSize(1);
  tft.print("Temperatura");
  tft.setCursor(25,140);
  tft.setTextSize(2);
  tft.print(t);
  tft.print(" C");
  tft.setCursor(90,138);
  tft.setTextSize(1);
  tft.print("o");
  
}

void umidade(){

  tft.drawBitmap(16,32,umidade1,96,96,UMID1);
  tft.drawBitmap(16,32,umidade2,96,96,UMID2);
  tft.drawBitmap(16,32,umidade3,96,96,PRETO);
  float h = dht.readHumidity();
  tft.setCursor (45,130);
  tft.setTextSize(1);
  tft.print("Umidade");
  tft.setCursor(32,140);
  tft.setTextSize(2);
  tft.print(h);
  tft.print("%");
}

void pressao(){

  tft.drawBitmap(16,32,pressao1,96,96,PRES1);
  tft.drawBitmap(16,32,pressao2,96,96,PRES2);
  tft.drawBitmap(16,32,pressao3,96,96,PRETO);
  float t = bmp.readPressure();
  tft.setCursor (45,130);
  tft.setTextSize(1);
  tft.print("Pressao");
  tft.setCursor(3,140);
  tft.setTextSize(2);
  tft.print(t/100);
  tft.print("hPa"); 
}


void previsao() {
  tft.setTextSize(1);

  if(millis() - millisPrevisao > 21600000){
    OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
    oneCallClient->setMetric(IS_METRIC);
    oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  
    long executionStart = millis();
    oneCallClient->update(&openWeatherMapOneCallData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
    delete oneCallClient;
    oneCallClient = nullptr;

    forecast1.observationTimestamp = openWeatherMapOneCallData.daily[1].dt;
    forecast1.tmin = openWeatherMapOneCallData.daily[1].tempMin;
    forecast1.tmax = openWeatherMapOneCallData.daily[1].tempMax;
    forecast1.cond = openWeatherMapOneCallData.daily[1].weatherMain;
    
    forecast2.observationTimestamp = openWeatherMapOneCallData.daily[2].dt;
    forecast2.tmin = openWeatherMapOneCallData.daily[2].tempMin;
    forecast2.tmax = openWeatherMapOneCallData.daily[2].tempMax;
    forecast2.cond = openWeatherMapOneCallData.daily[2].weatherMain;
    
    forecast3.observationTimestamp = openWeatherMapOneCallData.daily[3].dt;
    forecast3.tmin = openWeatherMapOneCallData.daily[3].tempMin;
    forecast3.tmax = openWeatherMapOneCallData.daily[3].tempMax;
    forecast3.cond = openWeatherMapOneCallData.daily[3].weatherMain;
    millisPrevisao = millis();
  }
  time_t time;


  drawForecast(30, 35, forecast1);
  
  drawForecast(75, 80, forecast2);

  drawForecast(120, 125, forecast3);
}

void drawForecast(int line, int col, Forecast forecast){
  time_t observationTimestamp = forecast.observationTimestamp;
  struct tm* timeInfo;
  timeInfo = localtime(&observationTimestamp);
  tft.setCursor(47,col);
  tft.print(WDAY_NAMES[timeInfo->tm_wday]);
  tft.print(" (");
  tft.print(timeInfo->tm_mday);
  tft.print("/");
  tft.print(MONTH_NAMES[timeInfo->tm_mon]);
  tft.print(")");
  tft.setCursor(47,col+10);
  tft.print("Min:");
  int tmin1 = forecast.tmin;
  tft.print(tmin1);
  tft.print(" Max:");
  int tmax1 = forecast.tmax;
  tft.print(tmax1);
  String cond = forecast.cond;
  
  if (cond == "Thunderstorm"){
    tft.setCursor(47,col+20);
    tft.print("Tempestade");
    tft.drawBitmap(3,line,tempestade1,48,48,TEM1);
    tft.drawBitmap(3,line,tempestade2,48,48,TEM2);
    tft.drawBitmap(3,line,tempestade3,48,48,TEM3);
    tft.drawBitmap(3,line,tempestade4,48,48,PRETO);
  }else if (cond == "Rain"){
    tft.setCursor(47,col+20);
    tft.print("Chuva");
    tft.drawBitmap(3,line,chuva1,48,48,CHU1);
    tft.drawBitmap(3,line,chuva2,48,48,CHU2);
    tft.drawBitmap(3,line,chuva3,48,48,PRETO);
  }else if(cond == "Drizzle"){
    tft.setCursor(47,col+20);
    tft.print("Pouca Chuva");
    tft.drawBitmap(3,line,chuva1,48,48,CHU1);
    tft.drawBitmap(3,line,chuva2,48,48,CHU2);
    tft.drawBitmap(3,line,chuva3,48,48,PRETO);
  }else if(cond == "Clouds"){
    tft.setCursor(47,col+20);
    tft.print("Nublado");
    tft.drawBitmap(3,line,nublado1,48,48,NUB1);
    tft.drawBitmap(3,line,nublado2,48,48,NUB2);
    tft.drawBitmap(3,line,nublado3,48,48,PRETO);
  }else if(cond == "Clear"){
    tft.setCursor(47, col+20);
    tft.print("Sol");
    tft.drawBitmap(3,line,sol1,48,48,SOL1);
    tft.drawBitmap(3,line,sol2,48,48,SOL2);
    tft.drawBitmap(3,line,sol3,48,48,PRETO);
  }
}

void resetClick(){
    if (digitalRead(PINO_RESET) == HIGH) {
       delay(20);
       
       tft.fillScreen(CHUMBO);
       tft.setTextSize(1);
       tft.setTextColor(BRANCO);
       tft.setCursor(25,70);
       tft.print("Configurando");
       tft.setTextSize(1);
       tft.setCursor(25,100);
       tft.print("rede");
       
       Serial.println("Abertura Portal"); //Abre o portal
       wifiManager.resetSettings();       //Apaga rede salva anteriormente
       delay(20);
            
       //callback para quando entra em modo de configuração AP
       wifiManager.setAPCallback(configModeCallback); 
       //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
       wifiManager.setSaveConfigCallback(saveConfigCallback); 
            
       if(!wifiManager.autoConnect("ESP_EST_METEOROLOGICA", "12345678") ){ //Nome da Rede e Senha gerada pela ESP     
           Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
           delay(2000);
           wifiManager.resetSettings();  
           ESP.reset(); //Reinicia ESP após não conseguir conexão na rede 
       }else{       //Se caso conectar 
           Serial.println("Conectado na Rede!!!");
           delay(1000);
           
           ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
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
