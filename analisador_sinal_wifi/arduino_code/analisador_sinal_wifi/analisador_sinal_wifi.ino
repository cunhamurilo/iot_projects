#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
 
#define PINO_LED   D1  // Pino do NodeMCU que o linha de LED está conectado
#define PINO_RESET 14
 
#define NUMERO_LED 8   // Número de LEDs na linha de LED
 
Adafruit_NeoPixel strip(NUMERO_LED, PINO_LED);
 
// Credeciais da rede WiFi
#define WIFI_SSID "" // Colocar SSID caso queria já iniciar já com os dados da internet
#define WIFI_PASS "" // Colocar PASSWORD caso queria já iniciar já com os dados da internet

WiFiManager wifiManager;//Objeto de manipulação do wi-fi
 
void setup() {
  Serial.begin(115200); 
  pinMode(PINO_RESET,INPUT);
  // Inicia a linha de LED
  strip.begin();
  strip.setBrightness(50); // O brilho da linha de LED pode ser ajustado aqui. Valores entre 0 e 255.
  strip.show();
  
  if (WiFi.SSID() != "") {
    // Inicia a conexão com a rede WiFi
    connectWiFi();
  }else{
    caseWiFi0();   
  }
    
}
 
void loop() {
  resetClick();
  
  if(WiFi.status() == WL_CONNECTED){
    static int Qualidade_anterior = -1;
    int qualidade = Qualidade_verificar();
    if (qualidade != Qualidade_anterior) {  
      // Se a qualidade do sinal WiFi mudou desde a última leitura, imprime a nova leitura
      if (qualidade != -1){
        if (qualidade <= 12) {
          caseWiFi0();
        }
         
        if (qualidade > 12 && qualidade <= 25){
          caseWiFi1();
        }
        
        if (qualidade > 25 && qualidade <= 37){
          caseWiFi2();
        }
         
        if (qualidade > 37 && qualidade <= 50){
          caseWiFi3();
        }
         
        if (qualidade > 50 && qualidade <= 62){
          caseWiFi4();
        }
         
        if (qualidade > 62 && qualidade <= 75){
          caseWiFi5();
        }
        
        if (qualidade > 75 && qualidade <= 87){
          caseWiFi6();
        }
         
        if (qualidade > 87){
          caseWiFi7();
        }     
       
        Qualidade_anterior = qualidade;
        delay(2000);
      }
    }
  }else{
     if(WiFi.SSID() != ""){
        connectWiFi();
     }
  }

  delay(100);

}

void connectWiFi(){
  Serial.println("Conectando");
  while ( WiFi.status() != WL_CONNECTED ) {
    resetClick();
    caseWiFi0();
    delay ( 300 );
    Serial.print ( "." );
    caseSetWiFi();
    delay ( 400 );
  }
  Serial.println("Conectado ao wifi");
}
 
void caseWiFi0(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 0, 0, 0);
  strip.setPixelColor(2, 0, 0, 0);
  strip.setPixelColor(3, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
       
void caseWiFi1(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 0, 0, 0);
  strip.setPixelColor(3, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
      
void caseWiFi2(){
 strip.setPixelColor(0, 255, 0, 0);
 strip.setPixelColor(1, 255, 0, 0);
 strip.setPixelColor(2, 255, 50, 0);
 strip.setPixelColor(3, 0, 0, 0);
 strip.setPixelColor(4, 0, 0, 0);
 strip.setPixelColor(5, 0, 0, 0);
 strip.setPixelColor(6, 0, 0, 0);
 strip.setPixelColor(7, 0, 0, 0);
 strip.show();
}
       
void caseWiFi3(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 50, 0);
  strip.setPixelColor(3, 255, 50, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
       
void caseWiFi4(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 50, 0);
  strip.setPixelColor(3, 255, 50, 0);
  strip.setPixelColor(4, 255, 255, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
       
void caseWiFi5(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 50, 0);
  strip.setPixelColor(3, 255, 50, 0);
  strip.setPixelColor(4, 255, 255, 0);
  strip.setPixelColor(5, 255, 255, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
      
void caseWiFi6(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 50, 0);
  strip.setPixelColor(3, 255, 50, 0);
  strip.setPixelColor(4, 255, 255, 0);
  strip.setPixelColor(5, 255, 255, 0);
  strip.setPixelColor(6, 0, 255, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();
}
 
void caseWiFi7(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 50, 0);
  strip.setPixelColor(3, 255, 50, 0);
  strip.setPixelColor(4, 255, 255, 0);
  strip.setPixelColor(5, 255, 255, 0);
  strip.setPixelColor(6, 0, 255, 0);
  strip.setPixelColor(7, 0, 255, 0);
  strip.show();
}


void caseSetWiFi(){
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(2, 255, 0, 0);
  strip.setPixelColor(3, 255, 0, 0);
  strip.setPixelColor(4, 255, 0, 0);
  strip.setPixelColor(5, 255, 0, 0);
  strip.setPixelColor(6, 255, 0, 0);
  strip.setPixelColor(7, 255, 0, 0);
  strip.show();
}
 
/*
Retorna a qualidade (indicador de intensidade do sinal recebido) da rede WiFi.
Retorna um número entre 0 e 100 se o WiFi estiver conectado.
Retorna -1 se o WiFi estiver desconectado.
*/
int Qualidade_verificar() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}


void resetClick(){
    if (digitalRead(PINO_RESET) == HIGH) {
       caseSetWiFi();
       delay(20);
       Serial.println("Abertura Portal"); //Abre o portal
       wifiManager.resetSettings();       //Apaga rede salva anteriormente
       delay(20);
            
       //callback para quando entra em modo de configuração AP
       wifiManager.setAPCallback(configModeCallback); 
       //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
       wifiManager.setSaveConfigCallback(saveConfigCallback); 
            
       if(!wifiManager.autoConnect("ESP_SINAL_WIFI", "12345678") ){ //Nome da Rede e Senha gerada pela ESP     
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
