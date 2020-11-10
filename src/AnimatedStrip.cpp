// Arduino Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "AnimatedStrip.h"

void AnimatedStrip::begin()
{
  m_numPixels = m_strip.numPixels();
}

void AnimatedStrip::setFixedMode() {
  m_state = FIXED;
}

void AnimatedStrip::setRainbowMode() {
  m_state = RAINBOW;
}

void AnimatedStrip::setWipeMode() {
  m_state = WIPE;
}

void AnimatedStrip::setRainbowCyclesMode() {
  m_state = RAINBOW_CYCLES;
}

void AnimatedStrip::setTheaterChaseMode() {
  m_state = THEATER_CHASE;
}

void AnimatedStrip::setTheaterChaseRainbowMode() {
  m_state = THEATER_CHASE_RAINBOW;
}

/**
 * Update with loop() method
 */
void AnimatedStrip::update() {
  switch (m_state)
  {
    case FIXED:
      // Nothing
      break;

    case WIPE:
      if ((unsigned long)(millis() - colorWipePreviousMillis) >= pixelsInterval) {
        colorWipePreviousMillis = millis();
        AnimatedStrip::colorWipe(m_strip.Color(0,255,125));
      }
      break;

    case THEATER_CHASE:
      if ((unsigned long)(millis() - theaterChasePreviousMillis) >= pixelsInterval) {
        theaterChasePreviousMillis = millis();
        AnimatedStrip::theaterChase(m_strip.Color(127, 127, 127)); // White
      }
      break;

    case THEATER_CHASE_RAINBOW:
      if ((unsigned long)(millis() - theaterChaseRainbowPreviousMillis) >= pixelsInterval) {
        theaterChaseRainbowPreviousMillis = millis();
        AnimatedStrip::theaterChaseRainbow();
      }
      break;

    case RAINBOW:
      if ((unsigned long)(millis() - rainbowPreviousMillis) >= pixelsInterval) {
        rainbowPreviousMillis = millis();
        AnimatedStrip::rainbow();
      }
      break;

    case RAINBOW_CYCLES:
      if ((unsigned long)(millis() - rainbowCyclesPreviousMillis) >= pixelsInterval) {
        rainbowCyclesPreviousMillis = millis();
        AnimatedStrip::rainbowCycle();
      }
      break;
  }
}

// Fill the dots one after the other with a color
void AnimatedStrip::colorWipe(uint32_t c) {
  m_strip.setPixelColor(m_pixel, c);
  m_strip.show();
  m_pixel++;

  if(m_pixel == m_strip.numPixels()) {
    m_pixel = 0;
  }
}

void AnimatedStrip::rainbow() {
  for(uint16_t i=0; i<m_numPixels; i++) {
    m_strip.setPixelColor(i, Wheel((i+rainbowCycles) & 255));
  }
  m_strip.show();
  rainbowCycles++;
  if(rainbowCycles >= 256) rainbowCycles = 0;
}

// Slightly different, this makes the rainbow equally distributed throughout
void AnimatedStrip::rainbowCycle() {
  uint16_t i;

  //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i<m_numPixels; i++) {
      m_strip.setPixelColor(i, Wheel(((i * 256 / m_strip.numPixels()) + rainbowCycleCycles) & 255));
    }
    m_strip.show();

  rainbowCycleCycles++;
  if(rainbowCycleCycles >= 256*5) rainbowCycleCycles = 0;
}

//Theatre-style crawling lights.
void AnimatedStrip::theaterChase(uint32_t c) {
  for (int i=0; i<m_numPixels; i=i+3) {
      m_strip.setPixelColor(i+theaterChaseQ, c);    //turn every third pixel on
  }

  m_strip.show();

  for (int i=0; i<m_numPixels; i=i+3) {
    m_strip.setPixelColor(i+theaterChaseQ, 0);        //turn every third pixel off
  }

  theaterChaseQ++;

  if(theaterChaseQ >= 3) theaterChaseQ = 0;
}


//Theatre-style crawling lights with rainbow effect
void AnimatedStrip::theaterChaseRainbow() {
  for (int i=0; i < m_strip.numPixels(); i=i+3) {
    m_strip.setPixelColor(i+theaterChaseRainbowQ, Wheel( (i+theaterChaseRainbowCycles) % 255));    //turn every third pixel on
  }
  m_strip.show();
  for (int i=0; i < m_strip.numPixels(); i=i+3) {
    m_strip.setPixelColor(i+theaterChaseRainbowQ, 0);        //turn every third pixel off
  }
  theaterChaseRainbowQ++;
  theaterChaseRainbowCycles++;
  if(theaterChaseRainbowQ >= 3) theaterChaseRainbowQ = 0;
  if(theaterChaseRainbowCycles >= 256) theaterChaseRainbowCycles = 0;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t AnimatedStrip::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return m_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return m_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return m_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}