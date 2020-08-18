#include <Servo.h>
#include <IRremote.h>
#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Ultrasonic.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;


int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
float armazenavalor;

const int stepsPerRevolution = 500;
Stepper stepperLefRight(stepsPerRevolution, 4, 6, 5, 7);
Stepper stepperUpDown(stepsPerRevolution, 8, 10, 9, 11);
Stepper stepperFrontBack(stepsPerRevolution, A3, A1, A2, A0);

#define SERVO 12

Servo s;

#define pino_trigger 3
#define pino_echo 13
Ultrasonic ultrasonic(pino_trigger, pino_echo);

unsigned long millisTarefa1 = millis();
unsigned long millisTarefa2 = millis();
bool near = false;


void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Inicializa o receptor IR
  stepperUpDown.setSpeed(60);
  stepperLefRight.setSpeed(60);
  stepperFrontBack.setSpeed(60);
  s.attach(SERVO);
  
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results))
  {
    Serial.print("Valor lido : ");
    Serial.println(results.value, HEX);
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
      s.write(140);
      // Check if close gar
    } else if (armazenavalor == 0xFF629D) {
      s.write(90);
    }

    irrecv.resume(); //Le o próximo valor
  }

  check_distance();

  delay(50);
}

void check_distance(){
  //Le as informacoes do sensor, em cm e pol
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  //Exibe informacoes no serial monitor
  //Serial.print("Distancia em cm: ");
  //Serial.println(cmMsec);
  //delay(1000);

  if(near == false){
    if(cmMsec < 15){
      Serial.println("Menor 15 cm");
      pwm.setPWM(0, 0, 150);
      millisTarefa1 = millis();
      near = true;
    }else{ 
      Serial.print("nao proximo  ");
      Serial.println(cmMsec);
    }
  }else{
    if((millis() - millisTarefa1) < 5000){
      Serial.println("Contando");
    }else{
      Serial.println("5 segundos");
      pwm.setPWM(0, 0, 300);
      near = false;
      //millisTarefa1 = millis();
    }
    if((millis() - millisTarefa1) > 5000){
      //millisTarefa1 = millis();
    }
    
  }
    
}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
