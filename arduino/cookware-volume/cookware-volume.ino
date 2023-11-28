int volumePin = A4;

void setup() {
    Serial.begin(9600);
    pinMode(volumePin, INPUT);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  serialOutVolume(volumePin);
  delay(15);
}

void serialOutVolume(int volpin) {
  int ar = analogRead(volpin);
  Serial.print("\nvolume pin read is ");
  Serial.println(ar);
}