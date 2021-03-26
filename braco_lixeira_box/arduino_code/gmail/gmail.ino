// Bibliotecas utilizadas
#include <AdafruitIO_WiFi.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
 
// Credenciais da conta Adafruit IO
#define IO_USERNAME     "your username adafruit io"
#define IO_KEY          "your key adafruit io"
 
// Credenciais da rede WiFi
#define WIFI_PASS       "your password"
String getNameSSID = WiFi.SSID();
const char *WIFI_SSID = getNameSSID.c_str(); // convert String to char
 
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
 
AdafruitIO_Feed *gmail_feed = io.feed("gmail");
 
WiFiManager wifiManager;  //Objeto de manipulação do wi-fi

String incomingByte = "";

void setup() {
   
  delay(2000);
  Serial.begin(115200);
  Serial.println("Notificador de e-mails do Gmail"); 

  if (WiFi.SSID() != "") {
    // Conexão inicial com a Adafruit IO
    connectWiFi();
    Serial.println();
   
    gmail_feed->get();
  }else{
    Serial.print("Sem rede");
  }
   
}
 
void loop() {
   resetClick();
  
   if(WiFi.status() == WL_CONNECTED){
      Serial.println("io run");
      io.run();
   }else{
      if(WiFi.SSID() != ""){
        connectWiFi();
      }
   }
   delay(10);
}
 
// Função chamada sempre que há uma novo dado na Adafruit IO
void handleMessage(AdafruitIO_Data *data) {
  Serial.write("new_email");
}

void connectWiFi(){
  Serial.println("Conectando com Adafruit IO");
  io.connect();
  gmail_feed->onMessage(handleMessage);
  while(io.status() < AIO_CONNECTED) {
    resetClick();
    delay ( 400 );
    Serial.print ( "." );
  }
  Serial.println("Conectado ao Adafruit IO");
  
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
            
          if(!wifiManager.autoConnect("ESP_TRASH", "12345678") ){ //Nome da Rede e Senha gerada pela ESP
            
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
