#include <NewPing.h>
#include <Servo.h>

#define TRIGGER_PIN_0 14
#define ECHO_PIN_0 15
#define MAX_DISTANCE 240

short flEnable = 5, frEnable = 9, blEnable = 10, brEnable = 11;
short flDir0 = 12, flDir1 = 13, frDir0 = 8, frDir1 = 6;
short blDir0 = 2, blDir1 = 3, brDir0 = 4, brDir1 = 7;
byte direction_ = 1, spd = 0;

bool check;

unsigned long dt, previousTime;

NewPing sonar0(TRIGGER_PIN_0, ECHO_PIN_0, MAX_DISTANCE);

void forward(byte motor = -1) {
  switch (motor) {
    case 0:
      digitalWrite(flDir0, HIGH);
      digitalWrite(flDir1, LOW);
      break;
    case 1:
      digitalWrite(frDir0, HIGH);
      digitalWrite(frDir1, LOW);
      break;
    case 2:
      digitalWrite(blDir0, HIGH);
      digitalWrite(blDir1, LOW);
      break;
    case 3:
      digitalWrite(brDir0, HIGH);
      digitalWrite(brDir1, LOW);
      break;
    default:
      digitalWrite(flDir0, HIGH);
      digitalWrite(flDir1, LOW);
      digitalWrite(frDir0, HIGH);
      digitalWrite(frDir1, LOW);
      digitalWrite(blDir0, HIGH);
      digitalWrite(blDir1, LOW);
      digitalWrite(brDir0, HIGH);
      digitalWrite(brDir1, LOW);
  }
}

void backward(byte motor = -1) {
  switch (motor) {
    case 0:
      digitalWrite(flDir0, LOW);
      digitalWrite(flDir1, HIGH);
      break;
    case 1:
      digitalWrite(frDir0, LOW);
      digitalWrite(frDir1, HIGH);
      break;
    case 2:
      digitalWrite(blDir0, LOW);
      digitalWrite(blDir1, HIGH);
      break;
    case 3:
      digitalWrite(brDir0, LOW);
      digitalWrite(brDir1, HIGH);
      break;
    default:
      digitalWrite(flDir0, LOW);
      digitalWrite(flDir1, HIGH);
      digitalWrite(frDir0, LOW);
      digitalWrite(frDir1, HIGH);
      digitalWrite(blDir0, LOW);
      digitalWrite(blDir1, HIGH);
      digitalWrite(brDir0, LOW);
      digitalWrite(brDir1, HIGH);
  }
}
void setup() {
  Serial.begin(9600);

  for (int i = 2; i <= 13; ++i) {
    pinMode(i, OUTPUT);
  }

  forward();

  Serial.println(F("I am alive.. "));
  Serial.println("Going Forward \n");

  Serial.println("---------------Hot Keys------------------");
  Serial.println(F("0-4 change dir indiv. motors --- 7 all reverse --- 9 all forward"));
  Serial.println(F("q power lvl 100 -- z power cut"));
  Serial.println(F("w increase power 10 -- s decrease power 10"));
  Serial.println(F("f begin sonar protocol"));
  Serial.println(F("m print status of motors 0-4 (fl, fr, bl, br)"));
  Serial.println("------------------------------------------");
  Serial.println();

}

void loop() {

  char hi;
  short bob = (sonar0.ping_cm() - 15) * 5;
  if (bob > 255) bob = 255;

  if (Serial.available()) hi = Serial.read();
  else hi = ' ';

  switch (hi) {
    case '9':
      forward();
      Serial.println("Going Forward");
      break;
    case '7':
      backward();
      Serial.println("Going Backward");
      break;
    case '0':
      if(digitalRead(flDir0) == HIGH) backward(0);
      else forward(0);
      Serial.println("FL motor direction change");
      break;
    case '1':
      if(digitalRead(frDir0) == HIGH) backward(1);
      else forward(1);
      Serial.println("FR motor direction change");
      break;
    case '2':
      if(digitalRead(blDir0) == HIGH) backward(2);
      else forward(2);
      Serial.println("BL motor direction change");
      break;
    case '3':
      if(digitalRead(brDir0) == HIGH) backward(3);
      else forward(3);
      Serial.println("BR motor direction change");
      break;
    case 'w':
      if (spd + 10 < 255) spd += 10;
      else spd = 255;
      Serial.print("Speed: ");
      Serial.println(spd);
      break;
    case 's':
      if (spd - 10 > 0) spd -= 10;
      else spd = 0;
      Serial.print("Speed: ");
      Serial.println(spd);
      break;
    case 'q':
      spd = 100;
      Serial.print("Speed: ");
      Serial.println(spd);
      break;
    case 'z':
      spd = 0;
      Serial.print("Speed: ");
      Serial.println(spd);
      break;
    case 'f':
      if (check == false) check = true;
      else check = false;
      break;
    case 'm':
      Serial.print(digitalRead(flDir0));
      Serial.print(", ");
      Serial.print(digitalRead(frDir0));
      Serial.print(", ");
      Serial.print(digitalRead(blDir0));
      Serial.print(", ");
      Serial.println(digitalRead(brDir0));

  }

//---------------------------------------------------------------------------//

  if (check == false) {
    analogWrite(flEnable, spd);
    analogWrite(frEnable, spd);
    analogWrite(blEnable, spd);
    analogWrite(brEnable, spd);
  } else {
    if (sonar0.ping_cm() < 20) {
      backward();
      bob = 240 - bob * 4;
      analogWrite(flEnable, bob);
      analogWrite(frEnable, bob);
      analogWrite(blEnable, bob);
      analogWrite(brEnable, bob);
    } else if (sonar0.ping_cm() > 15) {
      forward();
      analogWrite(flEnable, bob);
      analogWrite(frEnable, bob);
      analogWrite(blEnable, bob);
      analogWrite(brEnable, bob);
    }
    else {
      analogWrite(flEnable, 0);
      analogWrite(frEnable, 0);
      analogWrite(blEnable, 0);
      analogWrite(brEnable, 0);
    }
  }


  dt = micros() - previousTime;
  while (dt < 30000)
    dt = micros() - previousTime;
  previousTime = micros();

}





