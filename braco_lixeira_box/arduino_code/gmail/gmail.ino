// Bibliotecas utilizadas
#include <AdafruitIO_WiFi.h>
#include <ESP8266WiFi.h>
 
// Credenciais da conta Adafruit IO
#define IO_USERNAME     "your username"
#define IO_KEY          "your io_key"
 
// Credenciais da rede WiFi
#define WIFI_SSID       "your ssid"
#define WIFI_PASS       "your password"
 
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
 
AdafruitIO_Feed *gmail_feed = io.feed("gmail");
 
void setup() {
   
  Serial.begin(115200);
  Serial.print("Notificador de e-mails do Gmail"); 
 
  // Conexão inicial com a Adafruit IO
  Serial.print("Conectando com Adafruit IO");
  io.connect();
  gmail_feed->onMessage(handleMessage);
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
   
  Serial.println();
 
  gmail_feed->get();
   
}
 
void loop() {
   io.run();
}
 
// Função chamada sempre que há uma novo dado na Adafruit IO
void handleMessage(AdafruitIO_Data *data) {
  Serial.write("new_email");
}
