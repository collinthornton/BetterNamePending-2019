// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>

#define TRIGGER_PIN_0  5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_0     6
#define TRIGGER_PIN_1  7
#define ECHO_PIN_1     8// Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 240 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int enablePin = 11;
int dirPin1 = 10;
int dirPin2 = 9;

float pMotorOne, iMotorOne, dMotorOne, prevErrorMotorOne;

unsigned long dt, previousTime;

NewPing sonar0(TRIGGER_PIN_0, ECHO_PIN_0, MAX_DISTANCE);
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  pinMode(enablePin, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);

  Serial.println("I am alive... ");
}

void loop() {

  char hi = Serial.read();

  if(hi == 'p') {
    Serial.print("Ping0: ");
    Serial.print(sonar0.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.print("cm - Ping1: ");
    Serial.print(sonar1.ping_cm());
    Serial.print("cm - dt: ");
    Serial.println(((float)micros() - (float)previousTime) / 1000);    
  }

  if(hi == 'a') {
    if(digitalRead(dirPin1) == HIGH) {
      digitalWrite(dirPin1, LOW);
      digitalWrite(dirPin2, HIGH);
      Serial.print("dirPin1: ");
      Serial.print(digitalRead(dirPin1));
      Serial.print(", dirPin2: ");
      Serial.println(digitalRead(dirPin2));
    }
    else {
      digitalWrite(dirPin1, HIGH);
      digitalWrite(dirPin2, LOW);
      Serial.print("dirPin1: ");
      Serial.print(digitalRead(dirPin1));
      Serial.print(", dirPin2: ");
      Serial.println(digitalRead(dirPin2));
    }
  }

  pMotorOne = (int)(sonar0.ping_cm() - sonar1.ping_cm());
  iMotorOne += pMotorOne * ((float)dt / 1000000);
  dMotorOne = pMotorOne - prevErrorMotorOne;
  prevErrorMotorOne = pMotorOne;

  Serial.print(pMotorOne);
  Serial.print(" - ");
  Serial.print(iMotorOne);
  Serial.print(" - ");
  Serial.print(dMotorOne);
  Serial.print(" - ");
  Serial.println((float)dt / 1000000, 5);
  
  int motorOneSpeed = pMotorOne + iMotorOne + dMotorOne;
  if(motorOneSpeed < 0) motorOneSpeed = 0;
  else if(motorOneSpeed > 255) motorOneSpeed = 255;
  //analogWrite(enablePin, motorOneSpeed);

  dt = micros() - previousTime;
  while(dt < 30000)
    dt = micros() - previousTime;
  previousTime = micros();
  
}
