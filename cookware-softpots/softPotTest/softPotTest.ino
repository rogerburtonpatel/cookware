int pinNum1 = A0;
int pinNum2 = A1;
int pinNum3 = A2;
int pinNum4 = A3;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinNum1, INPUT);
  pinMode(pinNum2, INPUT);
  pinMode(pinNum3, INPUT);
  pinMode(pinNum4, INPUT);
  // attachInterrupt()
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  Serial.print("Rot. Lin. Pot Out: ");
  Serial.println(map(analogRead(pinNum1), 0, 1023, 0, 127));
  

  volatile int blueOut = analogRead(pinNum1);
  volatile int whiteOut = analogRead(pinNum2);
  volatile int yellowOut = analogRead(pinNum3);
  */

  delineate(pinNum1);
  delineate(pinNum2);
  delineate(pinNum3);
  delineate(pinNum4);
  Serial.println();
  
  /*
  Serial.print("Pin 2 Analog Value: ");
  Serial.println(analogRead(pinNum2));
  Serial.print("Pin 3 Analog Value: ");
  Serial.println(analogRead(pinNum3));
  */

  delay(15);
}

void delineate(int pin) {
  int ar = analogRead(pin);
  if (ar == 0) {
    Serial.print(0);
  } else if (ar > 0 && ar <= 79) {
      Serial.print(5);
  } else if (ar > 80 && ar <= 209) {
      Serial.print(4);
  } else if (ar > 210 && ar <= 509)  {
      Serial.print(3);
  } else if (ar > 510 && ar <= 840) {
      Serial.print(2);
  } else if (ar > 841 && ar <= 1023) { 
      Serial.print(1);
  }
  Serial.print(" ");
}
