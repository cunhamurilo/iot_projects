#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
 
#define PINO_LED   D1  // Pino do NodeMCU que o linha de LED está conectado
 
#define NUMERO_LED 8   // Número de LEDs na linha de LED
 
Adafruit_NeoPixel strip(NUMERO_LED, PINO_LED);
 
// Credeciais da rede WiFi
#define WIFI_SSID "your ssid"
#define WIFI_PASS "your password"
 
void setup() 
{
  // Inicia a conexão com a rede WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
   
  // Inicia a linha de LED
  strip.begin();
  strip.setBrightness(50); // O brilho da linha de LED pode ser ajustado aqui. Valores entre 0 e 255.
  strip.show();
}
 
void loop() 
{
  static int Qualidade_anterior = -1;
  int qualidade = Qualidade_verificar();
  if (qualidade != Qualidade_anterior) 
  {  
    // Se a qualidade do sinal WiFi mudou desde a última leitura, imprime a nova leitura
    if (qualidade != -1)
    {
      if (qualidade <= 12)
      {
        caseWiFi0();
      }
       
      if (qualidade > 12 && qualidade <= 25)
      {
        caseWiFi1();
      }
      
      if (qualidade > 25 && qualidade <= 37)
      {
        caseWiFi2();
      }
       
      if (qualidade > 37 && qualidade <= 50)
      {
        caseWiFi3();
      }
       
      if (qualidade > 50 && qualidade <= 62)
      {
        caseWiFi4();
      }
       
      if (qualidade > 62 && qualidade <= 75)
      {
        caseWiFi5();
      }
      
      if (qualidade > 75 && qualidade <= 87)
      {
        caseWiFi6();
      }
       
      if (qualidade > 87)
      {
        caseWiFi7();
      }     
     
    Qualidade_anterior = qualidade;
    delay(5000);
   }
  }
}
 
void caseWiFi0()
{
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
       
void caseWiFi1()
{
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
      
void caseWiFi2()
{
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
       
void caseWiFi3()
{
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
       
void caseWiFi4()
{
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
       
void caseWiFi5()
{
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
      
void caseWiFi6()
{
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
 
void caseWiFi7()
{
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
 
/*
Retorna a qualidade (indicador de intensidade do sinal recebido) da rede WiFi.
Retorna um número entre 0 e 100 se o WiFi estiver conectado.
Retorna -1 se o WiFi estiver desconectado.
*/
int Qualidade_verificar() 
{
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}
