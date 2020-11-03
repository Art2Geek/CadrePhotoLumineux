// Arduino Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "RgbMosfetStrip.h"

void RgbMosfetStrip::begin()
{
    pinMode(m_red_pin, OUTPUT);
    pinMode(m_green_pin, OUTPUT);
    pinMode(m_blue_pin, OUTPUT);
}

/**
 * Set RED brightness according to global brightness
 */
void RgbMosfetStrip::setRedBrightness(byte rBright) {
  analogWrite(m_red_pin, rBright * brightness / 100);
}

/**
 * Set GREEN brightness according to global brightness
 */
void RgbMosfetStrip::setGreenBrightness(byte gBright) {
  analogWrite(m_green_pin, gBright * brightness / 100);
}

/**
 * Set BLUE brightness according to global brightness
 */
void RgbMosfetStrip::setBlueBrightness(byte bBright) {
  analogWrite(m_blue_pin, bBright * brightness / 100);
}

/**
 * Change global brightness
 */
void RgbMosfetStrip::setBrightness(int brightness) {
    brightness = brightness;
}

/**
 * Change strip color using each LED brightness
 */
void RgbMosfetStrip::fill(byte rBright, byte gBright, byte bBright) {
  setRedBrightness(rBright);
  setGreenBrightness(gBright);
  setBlueBrightness(bBright);
}

/**
 * Change strip color using hexadecimal color
 */
void RgbMosfetStrip::fill(uint32_t hexColor) {
  setRedBrightness(hexColor >> 16);
  setGreenBrightness(hexColor >> 8 & 0xFF);
  setBlueBrightness(hexColor & 0xFF);
}

/**
 * Turn all LEDs off
 */
void RgbMosfetStrip::turnOff() {
    setRedBrightness(0);
    setGreenBrightness(0);
    setBlueBrightness(0);
}