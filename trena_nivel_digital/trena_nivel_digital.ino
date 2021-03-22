//Bibliotecas
#include<Wire.h>
#include <LiquidCrystal.h>

// Pinos
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
const int MPU = 0x68;
#define trigPin 8
#define echoPin 9
#define selectButton 10
#define laserPin 11

// Variáveis
int16_t AcX, AcY, AcZ;
long duration;
float distance;
int program = 0;
float d = 0;
float d1 = 0;
float d2 = 0;
int axis = 0;
int angle = 0;
int unitSelect = 0;
String unit = "cm";

void setup() {
  
  // Inicialização MPU6050
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  // Inicialização LCD 16x2
  lcd.begin(16, 2); 

  // Inicialização HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicialização Push-Button
  pinMode(selectButton, INPUT_PULLUP);

    // Inicialização Laser
  pinMode(laserPin, OUTPUT);

}

void loop() {
  
  switch (program) {

    //Programa de seleção da unidade de medida
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Selecione a ");
      lcd.setCursor(0, 1);
      lcd.print("unidade: ");
      lcd.setCursor(13, 1);
      lcd.print(unit);
      lcd.print("  ");
      delay(10);
      if (digitalRead(selectButton) == 0) {
        if (unitSelect == 0) {
          unit = "cm";
          unitSelect = 1;
        }
        else if (unitSelect == 1) {
          unit = "m";
          unitSelect = 0;
        }
        // Se o botão for pressionado por mais de meio segundo - mudar o programa
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 1;
          lcd.clear();
          delay(500);
        }
      }
      break;

    //Programa de medição de distância
    case 1:
      digitalWrite(laserPin, HIGH);
      distance = getDistance();
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      lcd.print(distance);
      lcd.print("    ");
      lcd.setCursor(14, 0);
      lcd.print(unit);
      delay(10);
      lcd.setCursor(0, 1);
      lcd.print("d:");
      lcd.setCursor(8, 1);
      lcd.print("d:");
      delay(200);
      
      // Salva a distância 1
      if (digitalRead(selectButton) == 0) {
        if (d == 0) {
          lcd.setCursor(0, 1);
          lcd.print("d:     ");
          lcd.setCursor(2, 1);
          lcd.print(distance);
          d = 1;
          delay(100);
        }
        // Salva a distância 2
        else if (d == 1) {
          lcd.setCursor(8, 1);
          lcd.print("d:     ");
          lcd.setCursor(10, 1);
          lcd.print(distance);
          d = 0;
          delay(100);
        }
        // Se o botão for pressionado por mais de meio segundo - mudar o programa
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 2;
          d = 0;
          lcd.clear();
          delay(500);
        }
      }
      break;

    // Programa de medição de ângulo
    case 2:
      digitalWrite(laserPin, LOW);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      AcX = Wire.read() << 8 | Wire.read();
      AcY = Wire.read() << 8 | Wire.read();
      AcZ = Wire.read() << 8 | Wire.read();
     
      if ( axis == 0) {
        // Cálculo do ângulo de inclinação
        angle = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
        lcd.setCursor(0, 0);
        lcd.print("Inclinacao");
      }
      else if (axis == 1) {
        // Calculo do ângulo de rotação
        angle = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;
        lcd.setCursor(0, 0);
        lcd.print("Rotacao");
      }
      lcd.setCursor(0, 1);
      lcd.print("Angulo: ");
      lcd.print(abs(angle));
      lcd.print("     ");
      lcd.setCursor(10, 1);
      lcd.print(" graus");
      delay(200);
      
      if (digitalRead(selectButton) == 0) {
        if (axis == 0) {
          axis = 1;
          delay(100);
        }
        
        else if (axis == 1) {
          axis = 0;
          delay(100);
        }
        //Se o botão for pressionado por mais de meio segundo - mudar o programa
        delay(500);
        if (digitalRead(selectButton) == 0) {
          program = 0;
          lcd.clear();
          delay(500);
        }
      }
      break;
  }
}

// Função para medição de distância
float getDistance() {
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; //distância em cm
  
  // Conversão de unidades
  if (unitSelect == 0) {
    distance = distance; // cm para cm
    unit = "cm";
  }
  else if (unitSelect == 1) {
    distance = distance * 0.01; // cm para m
    unit = "m";
  }
  return distance;
}
