#include <Arduino.h>

#define RED_LED 9
#define GREEN_LED 10
#define BLUE_LED 11

int brightness = 20; // 0 to 100

/**
 * Set RED brightness according to global brightness
 */
void setRedBrightness(byte rBright) {
  analogWrite(RED_LED, rBright * brightness / 100);
}

/**
 * Set GREEN brightness according to global brightness
 */
void setGreenBrightness(byte gBright) {
  analogWrite(GREEN_LED, gBright * brightness / 100);
}

/**
 * Set BLUE brightness according to global brightness
 */
void setBlueBrightness(byte bBright) {
  analogWrite(BLUE_LED, bBright * brightness / 100);
}

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  for (int i=0; i<256; i++) {
    setRedBrightness(i);
    delay(10);
  }

  for (int i=0; i<256; i++) {
    setGreenBrightness(i);
    delay(10);
  }

  for (int i=0; i<256; i++) {
    setBlueBrightness(i);
    delay(10);
  }

  for (int i=255; i>=0; i--) {
    setRedBrightness(i);
    setGreenBrightness(i);
    setBlueBrightness(i);
    delay(10);
  }
}