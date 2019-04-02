byte sIn = 6;
byte sOut = 5;

float readSonar() {
  digitalWrite(sOut, HIGH);
  delayMicroseconds(250);
  digitalWrite(sOut, LOW);

  long timer = micros();
  
  while(sIn == true) { Serial.println("I'm waiting"); }

  float seconds = ((float)micros() - (float)timer) / 1000000;
  float distance = 343 * seconds;
  return distance;

  
}

void setup() {
  // put your setup code here, to run once:

  pinMode(sIn, INPUT);
  pinMode(sOut, OUTPUT);

  digitalWrite(sOut, LOW);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.read() == 'w') {
    Serial.print("'w' Receieved: ");
    Serial.println(readSonar());
  }

}
