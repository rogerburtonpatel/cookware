/* Cookware full module. Flash once to give cookware full serial functionality.
   Authors: 
    damon prum, roger burtonpatel 
*/
/* BALANCE - XY PAD */
#include "Adafruit_TSC2007.h"
Adafruit_TSC2007 touch;

#define X_MIN 250 
#define Y_MIN 450

#define X_CENTER_LO 1460
#define X_CENTER_HI 2560
#define Y_CENTER_LO 1700
#define Y_CENTER_HI 3100

#define X_MAX 3900 
#define Y_MAX 3900 

#define PAD_CENTER 2000

enum Y_zone {lo,  y_center, hi};
enum X_zone {left, x_center, right};

enum Pad_out_region {
  pad_centercode,
  pad_sizzler_1, pad_sizzler_2,  pad_sizzler_3, pad_sizzler_4,
  pad_hicenter,  pad_leftcenter, pad_locenter,  pad_rightcenter
};
/* KEY - ROTARY KNOB CONTROLLERS */

#define rotaryPinA 2
#define rotaryPinB 3

volatile int knob_pos;
#define KNOB_MIN 0
#define KNOB_MAX 127

int initAPos;

unsigned long knobISR_last_run;
volatile bool pulseStart = false;


#define SW 4

/* SIZZLERS - LINEAR SOFTPOTS */
#define softPotPinNum1 A0
#define softPotPinNum2 A1
#define softPotPinNum3 A2
#define softPotPinNum4 A3

/* VOLUME - LOG KNOB CONTROLLERS */
#define volumePin A4

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
  // serialOutDeliniated(softPotPinNum1);
  // serialOutDeliniated(softPotPinNum2);
  // serialOutDeliniated(softPotPinNum3);
  // serialOutDeliniated(softPotPinNum4);
  // serialOutKeyInfo(); 
  serialOutXY(); // 0-9. read into max with js switch or array. 
  // serialOutVolume(volumePin);
  // serialOutButtonPress();
  Serial.println();
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
    const X_zone xzone = 
    x <= X_CENTER_LO ? right :
    x <  X_CENTER_HI ? x_center :
    left; 
    const Y_zone yzone = 
    y <= Y_CENTER_LO ? lo :
    y <  Y_CENTER_HI ? y_center :
    hi; 
    // Serial.println(xzone);
    // Serial.println(yzone);
    switch (xzone + 3 * yzone) {
      case 0: Serial.print(pad_sizzler_3);   break;
      case 1: Serial.print(pad_locenter);    break;
      case 2: Serial.print(pad_sizzler_4);   break;
      case 3: Serial.print(pad_leftcenter);  break;
      case 4: Serial.print(pad_centercode);  break;
      case 5: Serial.print(pad_rightcenter); break;
      case 6: Serial.print(pad_sizzler_2);   break;
      case 7: Serial.print(pad_hicenter);    break;
      case 8: Serial.print(pad_sizzler_1);   break;      
    }
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