#include <IRremote.h>
#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h> // Biblioteca SoftwareSerial

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
SoftwareSerial esp8266(2, 3); // objeto para comunicação serial com o ESP8266 nas Portas 2(RX) e 3(TX)

int RECV_PIN = 0;
int RESET_PIN = 1;

IRrecv irrecv(RECV_PIN);
decode_results results;
float armazenavalor;

const int stepsPerRevolution = 500;
Stepper stepperLefRight(stepsPerRevolution, 4, 6, 5, 7);
Stepper stepperUpDown(stepsPerRevolution, 8, 10, 9, 11);
Stepper stepperFrontBack(stepsPerRevolution, A3, A1, A2, A0);  // voltar para A0

#define pino_trigger 13
#define pino_echo 12
Ultrasonic ultrasonic(pino_trigger, pino_echo);

unsigned long millisTarefa1 = millis();
unsigned long millisEmail = 0;
bool near = false;

void setup() {
//  /Serial.begin(115200); // inicia a comunicação serial com o monitor
  esp8266.begin(115200); // inicia a comunicação serial com o ESP8266

  irrecv.enableIRIn(); // Inicializa o receptor IR

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_PIN, INPUT);

  stepperUpDown.setSpeed(60);
  stepperLefRight.setSpeed(60);
  stepperFrontBack.setSpeed(60);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  pwm.setPWM(15, 0, 300);
}

void loop() {

  check_Ir();
  check_distance();
  check_serial();

  if(digitalRead(RESET_PIN)){
    esp8266.write("reset");
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(50);
}

void check_Ir() {
  if (irrecv.decode(&results)) {
//    Serial.print("Valor lido : ");
//    Serial.println(results.value, HEX);
    armazenavalor = (results.value);
    // Check if up
    if (armazenavalor == 0xFF18E7) {
      stepperUpDown.step(200);
      // Check if down
    } else if (armazenavalor == 0xFF4AB5) {
      stepperUpDown.step(-200);
      // Check if right
    } else if (armazenavalor == 0xFF5AA5) {
      stepperLefRight.step(200);
      // Check if left
    } else if (armazenavalor == 0xFF10EF) {
      stepperLefRight.step(-200);
      // Check if front
    } else if (armazenavalor == 0xFF6897) {
      stepperFrontBack.step(200);
      // Check if back
    } else if (armazenavalor == 0xFFB04F) {
      stepperFrontBack.step(-200);
      // Check if open gar
    } else if (armazenavalor == 0xFFA25D) {
      pwm.setPWM(4, 0, 300);
      // Check if close gar
    } else if (armazenavalor == 0xFF629D) {
      pwm.setPWM(4, 0, 150);
    }

    irrecv.resume(); //Le o próximo valor
  }
}

void check_distance() {
  //Le as informacoes do sensor, em cm e pol
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //Exibe informacoes no serial monitor
  //Serial.print("Distancia em cm: ");
  //Serial.println(cmMsec);
  //delay(1000);

  if (near == false) {
    if (cmMsec < 15) {
//      Serial.println("Menor 15 cm");
      pwm.setPWM(0, 0, 150);
      millisTarefa1 = millis();
      near = true;
    } else {
//      Serial.print("nao proximo  ");
//      Serial.println(cmMsec);
    }
  } else {
    if ((millis() - millisTarefa1) > 5000) {
//      Serial.println("5 segundos");
      pwm.setPWM(0, 0, 300);
      near = false;
      millisTarefa1 = 0;
    }
  }

}

void check_serial() {
  if (esp8266.available()) {
    String resp = "";
    while (esp8266.available()){
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      resp += c;
    }

//    Serial.println(resp);
    if (resp.compareTo("new_email")) {
      pwm.setPWM(15, 0, 150);
      pwm.setPWM(14, 0, 150);
      millisEmail = millis();
    }
  }

  if ( (millis() - millisEmail) > 1800000 ) {
    pwm.setPWM(15, 0, 300);
    millisEmail = 0;
  }
}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
//  Serial.print(pulselength); 
//  Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
//  Serial.print(pulselength); 
//  Serial.println(" us per bit");
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
//  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
