/* Cookware full module. Flash once to give cookware full serial functionality.
   Authors: 
    damon prum, roger burtonpatel 
*/
/* BALANCE - XY PAD */
#include "Adafruit_TSC2007.h"
Adafruit_TSC2007 touch;

#define X_MIN 250 
#define Y_MIN 450
#define X_MAX 3900 
#define Y_MAX 3900 
#define PAD_CENTER 2000

/* KEY - ROTARY KNOB CONTROLLERS */

#define rotaryPinA 2
#define rotaryPinB 3

volatile int knob_pos;
const int KNOB_MIN = 0;
const int KNOB_MAX = 127;

int initAPos;


unsigned long knobISR_last_run;
volatile bool pulseStart = false;


#define SW 4

/* SIZZLERS - LINEAR SOFTPOTS */
int softPotPinNum1 = A0;
int softPotPinNum2 = A1;
int softPotPinNum3 = A2;
int softPotPinNum4 = A3;

/* VOLUME - LOG KNOB CONTROLLERS */
int volumePin = A4;

void setup() {
    Serial.begin(9600);

    /* KEY - ROTARY KNOB CONTROLLERS */
    pinMode(rotaryPinA, INPUT_PULLUP);
    pinMode(rotaryPinB, INPUT_PULLUP);
    pinMode(SW, INPUT);
    knob_pos = 0;
    knobISR_last_run = 0;
    attachInterrupt(digitalPinToInterrupt(rotaryPinA), knobISR, CHANGE);

    /* VOLUME - LOG KNOB CONTROLLERS */
    pinMode(volumePin, INPUT);

    /* SIZZLERS - LINEAR SOFTPOTS */
    pinMode(softPotPinNum1, INPUT);
    pinMode(softPotPinNum2, INPUT);
    pinMode(softPotPinNum3, INPUT);
    pinMode(softPotPinNum4, INPUT);

    if (!touch.begin()) {
        Serial.println("Couldn't find touch controller");
        delay(1000);
      }
}

void loop() {
  serialOutDeliniated(softPotPinNum1);
  serialOutDeliniated(softPotPinNum2);
  serialOutDeliniated(softPotPinNum3);
  serialOutDeliniated(softPotPinNum4);
  serialOutKeyInfo(); 
  serialOutXY();
  serialOutVolume(volumePin);
  serialOutButtonPress();
  Serial.println();
    // Serial.print(x); Serial.print(", ");
    // Serial.print(y); Serial.print(", ");
    // Serial.print(z1); Serial.print(" / ");
    // Serial.print(z2); Serial.println(")");
  delay(50);
}

void serialOutVolume(int volpin) {
  int ar = analogRead(volpin);
  // Serial.print("\nvolume pin read is ");
  Serial.print(ar);
  Serial.print(" ");
}

void serialOutXY() {
  uint16_t x, y, z1, z2;
  if (touch.read_touch(&x, &y, &z1, &z2)) {
    if (x < PAD_CENTER) {
      if (y < PAD_CENTER) { 
          Serial.print(4);
      } else { 
          Serial.print(1); 
        }
    } else if (y < PAD_CENTER) {
          Serial.print(3); 
    } else {
          Serial.print(2); 
    }
  } else {
          Serial.print(0);
  }
  Serial.print(" ");
}

void serialOutDeliniated(int pin) {
  int ar = analogRead(pin);
  // Serial.println(ar);
  if (ar == 0) {
    Serial.print(0);
  } else if (ar > 0 && ar <= 139) {
      Serial.print(5);
  } else if (ar > 140 && ar <= 299) {
      Serial.print(4);
  } else if (ar > 300 && ar <= 479)  {
      Serial.print(3);
  } else if (ar > 480 && ar <= 739) {
      Serial.print(2);
  } else if (ar > 740 && ar <= 1023) { 
      Serial.print(1);
  }
  Serial.print(" ");
}

/* helper for rotary encoder. */
void knobISR() {
  // 5ms buffer
  if (millis() - knobISR_last_run > 5) {
    // suggests clockwise
    if (digitalRead(rotaryPinA) != digitalRead(rotaryPinB)) {
        knob_pos = min(knob_pos + 7, KNOB_MAX);
    // else, we're counter-clockwise
    } else {
        knob_pos = max(knob_pos - 7, KNOB_MIN);
    }
  }
  knobISR_last_run = millis();
}

/* prints key info based on rotary encoder. caps at 20. */
void serialOutKeyInfo() {
  Serial.print(knob_pos);
  Serial.print(" ");
}

void serialOutButtonPress() {
  int btnState = digitalRead(SW);
  Serial.print(btnState);
  Serial.print(" ");
}