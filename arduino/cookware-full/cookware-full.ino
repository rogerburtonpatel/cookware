/* Cookware full module. Flash once to give cookware full serial functionality.
   Authors: 
    damon prum, roger burtonpatel 
*/

/* LEDs */
#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    44
#define INITIAL_BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

const uint32_t CustomOrange = 0xFF3800;

const struct CRGB POSSIBLE_COLORS[24] = {
  CRGB::DarkRed,    CRGB::DarkRed, 
  CRGB::DarkRed,    CustomOrange, 
  CustomOrange,     CustomOrange, 
  CRGB::Yellow,     CRGB::Yellow,
  CRGB::Yellow,     CRGB::Green,
  CRGB::Green,      CRGB::Green,
  CRGB::Green,      CRGB::Blue, 
  CRGB::Blue,       CRGB::Blue, 
  CRGB::Purple,     CRGB::Purple, 
  CRGB::Purple,     CRGB::LightPink, 
  CRGB::LightPink,  CRGB::LightPink, 
  CRGB::LightPink,  CRGB::DarkRed
};

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


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
  pad_noop,
  pad_sizzler_1, pad_sizzler_2,  pad_sizzler_3, pad_sizzler_4,
  pad_hicenter,  pad_leftcenter, pad_locenter,  pad_rightcenter,
  pad_centercode
};
/* KEY - ROTARY KNOB CONTROLLERS */

#include "Adafruit_seesaw.h"

#define SS_SWITCH   24
#define SEESAW_ADDR 0x36

Adafruit_seesaw ss;

int32_t encoder_position;

// #define rotaryPinA 2
// #define rotaryPinB 3

#define KNOB_MIN 1
#define KNOB_MAX 12
volatile int32_t key_knob_pos = KNOB_MIN;

// int32_t initAPos;

// uint64_t knobISR_last_run;
// volatile bool pulseStart = false;

// #define SW 4

/* SIZZLERS - LINEAR SOFTPOTS */
#define softPotPinNum1 A0
#define softPotPinNum2 A1
#define softPotPinNum3 A2
#define softPotPinNum4 A3

/* VOLUME - LOG KNOB CONTROLLERS */
#define volumePin A4
uint8_t volumePos = 64; 

void setup() {
    Serial.begin(9600);


    /* KEY - ROTARY KNOB CONTROLLERS */
    if (!ss.begin(SEESAW_ADDR)) {
      Serial.println("Couldn't find seesaw rotary encoder on default address");
      while(1) delay(10);
    }

    ss.pinMode(SS_SWITCH, INPUT_PULLUP);

    encoder_position = ss.getEncoderPosition();
    ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, true);
    ss.enableEncoderInterrupt();
    key_knob_pos = 0;

    /* LEDS */
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;


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

    /* boot sequence */
    FastLED.setBrightness(0);
    FastLED.show();
    /* hang until I press a button */ 
    while (ss.digitalRead(SS_SWITCH)) {}
    /* give a little show */
    FastLED.setBrightness(  INITIAL_BRIGHTNESS );
    initLights();
}

void loop() {
  readAndOutputLinearSoftpot(softPotPinNum1);
  readAndOutputLinearSoftpot(softPotPinNum2);
  readAndOutputLinearSoftpot(softPotPinNum3);
  readAndOutputLinearSoftpot(softPotPinNum4);
  readAndOutputKeyInfo(); 
  readAndOutputXY(); // 0-9. read into max with js switch or array. 
  readAndOutputVolume(volumePin);
  readAndOutputButtonPress();
  Serial.println();
  leds_on();
  delay(25);
}

void readAndOutputVolume(int volpin) {
  volumePos = analogRead(volpin) / 8; // range reduction from 1023 to 127
  Serial.print(volumePos);
  Serial.print(" ");
}

void readAndOutputXY() {
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
    // Gives desired offset pattern in relatively few LOC
    switch (xzone + 3 * yzone) {
      case 0: Serial.print(pad_sizzler_1);   break;
      case 1: Serial.print(pad_rightcenter); break; 
      case 2: Serial.print(pad_sizzler_4);   break;
      case 3: Serial.print(pad_hicenter);    break; 
      case 4: Serial.print(pad_centercode);  break;
      case 5: Serial.print(pad_locenter);    break;
      case 6: Serial.print(pad_sizzler_2);   break;
      case 7: Serial.print(pad_leftcenter);  break;
      case 8: Serial.print(pad_sizzler_3);   break;
    }
  } else {
    Serial.print(0);
  }
  Serial.print(" ");
}

void readAndOutputLinearSoftpot(int pin) {
  int32_t ar = analogRead(pin);
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

/* prints key info based on rotary encoder. caps at 12, floors at 1. */
void readAndOutputKeyInfo() {
  int32_t new_position = ss.getEncoderPosition();

  if (new_position > encoder_position) {
    key_knob_pos = min(key_knob_pos + 1, KNOB_MAX);
  } else if (new_position < encoder_position) {
    key_knob_pos = max(key_knob_pos - 1, KNOB_MIN);
  }
  encoder_position = new_position; 
  Serial.print(key_knob_pos);
  Serial.print(" ");
}

void readAndOutputButtonPress() {
  if (! ss.digitalRead(SS_SWITCH)) {
    Serial.print(1);
  } else {
    Serial.print(0);
  }
  Serial.print(" ");
}

void leds_on() {

    uint8_t brightness = volumePos;
    uint8_t key = max(0, key_knob_pos - 1);

    FastLED.setBrightness(brightness);
    const uint8_t halfway_point = NUM_LEDS / 2 - 6;
    fill_solid(&leds[0], halfway_point, POSSIBLE_COLORS[key * 2]);  
    fill_solid(&leds[halfway_point], NUM_LEDS - halfway_point, POSSIBLE_COLORS[key * 2 + 1]);  
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

void FillLEDsFromPaletteColors( uint8_t colorIndex, uint8_t brightness)
{
    // uint8_t brightness = volumePos;
    for(uint8_t i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


void initLights() 
{
  uint64_t time_end = millis() + 7000;

  float brightness = 4.0; 
  const float correction = 255.0 / 400.0;
  while (millis() < time_end) {
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex, (uint8_t) (brightness * correction));
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
    brightness = min(400, brightness + 1);
    /* bress the button, skip the sequence. */
    if (time_end - millis() < 6500 && !ss.digitalRead(SS_SWITCH)) {
      break;
    }
  }
}