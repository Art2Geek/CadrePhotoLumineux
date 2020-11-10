// Arduino Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef ANIMATED_STRIP_H_INCLUDED
#define ANIMATED_STRIP_H_INCLUDED

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class AnimatedStrip
{
    public:
        // Button(strip, brightness) instantiates a Animated Strip object.
        //
        // Required parameter:
        // pin      The Arduino pin the strip is connected to
        //
        // Optional parameters:
        // brightness  Global brightness (default 100)
        AnimatedStrip(Adafruit_NeoPixel strip) : m_strip(strip) {}

        // Initialize a RGB Mosfet Strip object and the pins connected to
        void begin();

        /**
         * Update with loop() method
         */
        void update();

        void setFixedMode();
        void setWipeMode();
        void setRainbowCyclesMode();
        void setRainbowMode();
        void setTheaterChaseMode();
        void setTheaterChaseRainbowMode();

        void colorWipe(uint32_t c);

        void rainbow();

        void rainbowCycle();

        void theaterChase(uint32_t c);

        void theaterChaseRainbow();

        uint32_t Wheel(byte WheelPos);

    private:
        Adafruit_NeoPixel m_strip;      // Neopixel strip
        // List of possible states for the state machine. This state machine has a fixed
        enum states_t {
            FIXED,
            WIPE,
            THEATER_CHASE,
            THEATER_CHASE_RAINBOW,
            RAINBOW,
            RAINBOW_CYCLES
        };

        states_t m_state; // current state machine state
        uint16_t m_pixel = 0; // Current pixel
        int m_numPixels;

        uint32_t pixelsInterval=150;  // the time we need to wait
        uint32_t colorWipePreviousMillis=0;
        uint32_t theaterChasePreviousMillis=0;
        uint32_t theaterChaseRainbowPreviousMillis=0;
        uint32_t rainbowPreviousMillis=0;
        uint32_t rainbowCyclesPreviousMillis=0;


        int theaterChaseQ = 0;
        int theaterChaseRainbowQ = 0;
        int theaterChaseRainbowCycles = 0;
        int rainbowCycles = 0;
        int rainbowCycleCycles = 0;
};
#endif