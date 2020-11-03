// Arduino Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef RGB_MOSFET_STRIP_H_INCLUDED
#define RGB_MOSFET_STRIP_H_INCLUDED

#include <Arduino.h>

class RgbMosfetStrip
{
    public:
        // Button(redPin, greenPin, bluePin, brightness) instantiates a RGB Mosfet Strip object.
        //
        // Required parameter:
        // redPin      The Arduino pin the red LEDs are connected to
        // greenPin    The Arduino pin the green LEDs are connected to
        // bluePin     The Arduino pin the blue LEDs are connected to
        //
        // Optional parameters:
        // brightness  Global brightness (default 100)
        RgbMosfetStrip(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, int brightness=100)
            : m_red_pin(redPin), m_green_pin(greenPin), m_blue_pin(bluePin), brightness(brightness) {}

        // Initialize a RGB Mosfet Strip object and the pins connected to
        void begin();

        /**
         * Set RED brightness according to global brightness
         */
        void setRedBrightness(byte rBright);

        /**
         * Set GREEN brightness according to global brightness
         */
        void setGreenBrightness(byte gBright);

        /**
         * Set BLUE brightness according to global brightness
         */
        void setBlueBrightness(byte bBright);

        /**
         * Change global brightness
         */
        void setBrightness(int value);

        /**
         * Change strip color using each LED brightness
         */
        void fill(byte rBright, byte gBright, byte bBright);

        /**
         * Change strip color using hexadecimal color
         */
        void fill(uint32_t hexColor);

        /**
         * Turn all LEDs off
         */
        void turnOff();

    private:
        uint8_t m_red_pin;      // arduino pin number connected to red LEDs
        uint8_t m_green_pin;    // arduino pin number connected to green LEDs
        uint8_t m_blue_pin;     // arduino pin number connected to blue LEDs
        int brightness;        // global brightness (0 to 100)
};
#endif