#include <Arduino.h>
#include <RgbMosfetStrip.h>
#include <AnimatedStrip.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define GREEN_LED 3
#define RED_LED 5
#define BLUE_LED 6

#define SEA_STRIP 2
#define BRIDGE_BOTTOM_STRIP 7
#define BRIDGE_TOP_STRIP 14
#define TOWERS_LEFT 8
#define TOWERS_RIGHT 10

#define SEA_PIXELS 46
#define BRIDGE_BOTTOM_PIXELS 33 // Normaly 37 but better render with 33
#define BRIDGE_TOP_PIXELS 12
#define TOWERS_LEFT_PIXELS 1
#define TOWERS_RIGHT_PIXELS 4

String btString = "";

RgbMosfetStrip ambiantStrip = RgbMosfetStrip(RED_LED, GREEN_LED, BLUE_LED);
Adafruit_NeoPixel seaStrip = Adafruit_NeoPixel(SEA_PIXELS, SEA_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeBottomStrip = Adafruit_NeoPixel(BRIDGE_BOTTOM_PIXELS, BRIDGE_BOTTOM_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeTopStrip = Adafruit_NeoPixel(BRIDGE_TOP_PIXELS, BRIDGE_TOP_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersLeftStrip = Adafruit_NeoPixel(TOWERS_LEFT_PIXELS, TOWERS_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersRightStrip = Adafruit_NeoPixel(TOWERS_RIGHT_PIXELS, TOWERS_RIGHT, NEO_GRB + NEO_KHZ800);

// List of possible states for the state machine. This state machine has a fixed
enum states_t {
  FIXED,
  // WIPE,
  THEATER_CHASE,
  // THEATER_CHASE_RAINBOW,
  // RAINBOW,
  RAINBOW_CYCLES
};

states_t states[5] = {FIXED, FIXED, FIXED, FIXED, FIXED}; // current state machine state

int rainbowCycleCycles[5] = {0, 0, 0, 0, 0};
unsigned  theaterChaseQ[5] = {0, 0, 0, 0, 0};
unsigned long theaterChasePreviousMillis[5] = {0, 0, 0, 0, 0};
unsigned long rainbowCyclesPreviousMillis[5] = {0, 0, 0, 0, 0};
unsigned long pixelsInterval=150;  // the time we need to wait

AnimatedStrip animatedSeaStrip = AnimatedStrip(seaStrip);
AnimatedStrip animatedBridgeBottomStrip = AnimatedStrip(bridgeBottomStrip);
AnimatedStrip animatedBridgeTopStrip = AnimatedStrip(bridgeTopStrip);

SoftwareSerial BTSerial(15, 16);

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void changeColor(String inputString) {
  String strip = getValue(inputString, ',', 1);
  int red = getValue(inputString, ',', 2).toInt();
  int green = getValue(inputString, ',', 3).toInt();
  int blue = getValue(inputString, ',', 4).toInt();

  if (strip == "A") {
    ambiantStrip.fill(red, green, blue);
  } else if (strip == "S") {
    seaStrip.fill(seaStrip.Color(red, green, blue), 0, seaStrip.numPixels());
    seaStrip.show();
  } else if (strip == "BB") {
    bridgeBottomStrip.fill(bridgeBottomStrip.Color(red, green, blue), 0, bridgeBottomStrip.numPixels());
    bridgeBottomStrip.show();
  } else if (strip == "BT") {
    bridgeTopStrip.fill(bridgeTopStrip.Color(red, green, blue), 0, bridgeTopStrip.numPixels());
    bridgeTopStrip.show();
  } else if (strip == "TL") {
    towersLeftStrip.fill(towersLeftStrip.Color(red, green, blue), 0, towersLeftStrip.numPixels());
    towersLeftStrip.show();
  } else if (strip == "TR") {
    towersRightStrip.fill(towersRightStrip.Color(red, green, blue), 0, towersRightStrip.numPixels());
    towersRightStrip.show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(Adafruit_NeoPixel strip, byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
// void colorWipe(Adafruit_NeoPixel strip, uint32_t c) {
//   strip.setPixelColor(m_pixel, c);
//   strip.show();
//   m_pixel++;

//   if(m_pixel == strip.numPixels()) {
//     m_pixel = 0;
//   }
// }

// void rainbow(Adafruit_NeoPixel strip) {
//   for(uint16_t i=0; i<strip.numPixels(); i++) {
//     strip.setPixelColor(i, Wheel((i+rainbowCycles) & 255));
//   }
//   strip.show();
//   rainbowCycles++;
//   if(rainbowCycles >= 256) rainbowCycles = 0;
// }

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(int index, Adafruit_NeoPixel strip) {
  uint16_t i;

  int _rainbowCycleCycles = rainbowCycleCycles[index];

  //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(strip, ((i * 256 / strip.numPixels()) + _rainbowCycleCycles) & 255));
    }
    strip.show();

  _rainbowCycleCycles++;
  if(_rainbowCycleCycles >= 256*5) {
    _rainbowCycleCycles = 0;
  }

  rainbowCycleCycles[index] = _rainbowCycleCycles;
}

//Theatre-style crawling lights.
void theaterChase(int index, Adafruit_NeoPixel strip, uint32_t c) {
  int _theaterChaseQ = theaterChaseQ[index];

  for (int i=0; i<strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+_theaterChaseQ, c);    //turn every third pixel on
  }

  strip.show();

  for (int i=0; i<strip.numPixels(); i=i+3) {
    strip.setPixelColor(i+_theaterChaseQ, 0);        //turn every third pixel off
  }

  _theaterChaseQ++;

  if(_theaterChaseQ >= 3) {
    _theaterChaseQ = 0;
  }

  theaterChaseQ[index] = _theaterChaseQ;
}


//Theatre-style crawling lights with rainbow effect
// void theaterChaseRainbow(Adafruit_NeoPixel strip) {
//   for (int i=0; i < strip.numPixels(); i=i+3) {
//     strip.setPixelColor(i+theaterChaseRainbowQ, Wheel( (i+theaterChaseRainbowCycles) % 255));    //turn every third pixel on
//   }
//   strip.show();
//   for (int i=0; i < strip.numPixels(); i=i+3) {
//     strip.setPixelColor(i+theaterChaseRainbowQ, 0);        //turn every third pixel off
//   }
//   theaterChaseRainbowQ++;
//   theaterChaseRainbowCycles++;
//   if(theaterChaseRainbowQ >= 3) theaterChaseRainbowQ = 0;
//   if(theaterChaseRainbowCycles >= 256) theaterChaseRainbowCycles = 0;
// }

void mode0() {
  Serial.println("Mode 0");

  // animatedSeaStrip.setFixedMode();
  // animatedBridgeBottomStrip.setFixedMode();
  // animatedBridgeTopStrip.setFixedMode();

  // seaStrip.clear();
  seaStrip.fill(seaStrip.Color(0, 127, 255), 0, SEA_PIXELS);
  seaStrip.show();

  bridgeBottomStrip.fill(bridgeBottomStrip.Color(127, 127, 127), 0, BRIDGE_BOTTOM_PIXELS);
  bridgeBottomStrip.show();

  bridgeTopStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_TOP_PIXELS);
  bridgeTopStrip.show();

  towersLeftStrip.fill(towersLeftStrip.Color(0, 0, 255), 0, TOWERS_LEFT_PIXELS);
  towersLeftStrip.show();

  towersRightStrip.fill(towersRightStrip.Color(255, 0, 0), 0, TOWERS_RIGHT_PIXELS);
  towersRightStrip.show();
}

void mode1() {
  Serial.println("Mode 1");

  // animatedSeaStrip.setRainbowCyclesMode();
  // animatedBridgeBottomStrip.setTheaterChaseMode();
  // animatedBridgeTopStrip.setTheaterChaseMode();

  // for (int i=0; i<seaStrip.numPixels(); i++) {
  //   seaStrip.setPixelColor(i, Wheel(seaStrip, ((i * 256 / seaStrip.numPixels())) & 255));
  // }
  states[0] = RAINBOW_CYCLES;
  seaStrip.show();


  // bridgeTopStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_TOP_PIXELS);
  states[1] = RAINBOW_CYCLES;
  bridgeTopStrip.show();


  // bridgeBottomStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_BOTTOM_PIXELS);
  states[2] = THEATER_CHASE;
  bridgeBottomStrip.show();


  towersLeftStrip.setPixelColor(0, towersLeftStrip.Color(0, 0, 255));
  towersLeftStrip.show();

  towersRightStrip.setPixelColor(0, towersRightStrip.Color(255, 0, 0));
  towersRightStrip.fill(towersRightStrip.Color(127, 127, 127), 1, 2);
  towersRightStrip.setPixelColor(3, towersRightStrip.Color(255, 0, 0));
  towersRightStrip.show();
}

void mode2() {
  Serial.println("Mode 2");

  seaStrip.fill(seaStrip.Color(255, 255, 255), 0, SEA_PIXELS);
  seaStrip.show();

  bridgeBottomStrip.fill(bridgeBottomStrip.Color(255, 255, 255), 0, bridgeBottomStrip.numPixels());
  bridgeBottomStrip.show();

  bridgeTopStrip.fill(bridgeTopStrip.Color(255, 255, 255), 0, bridgeTopStrip.numPixels());
  bridgeTopStrip.show();

  towersLeftStrip.setPixelColor(0, towersLeftStrip.Color(255, 255, 255));
  towersLeftStrip.show();

  towersRightStrip.setPixelColor(0, towersRightStrip.Color(127, 127, 127));
  towersRightStrip.fill(towersRightStrip.Color(127, 127, 127), 1, 2);
  towersRightStrip.setPixelColor(3, towersRightStrip.Color(127, 127, 127));
  towersRightStrip.show();
}

void changeMode(String inputString) {
  int mode = getValue(inputString, ',', 1).toInt();

  // Serial.print("Change mode : ");
  // Serial.println(mode);

  seaStrip.clear();
  seaStrip.show();

  bridgeBottomStrip.clear();
  bridgeBottomStrip.show();

  bridgeTopStrip.clear();
  bridgeTopStrip.show();

  towersLeftStrip.clear();
  towersLeftStrip.show();

  towersRightStrip.clear();
  towersRightStrip.show();

  if (mode == 0) {
    mode0();
  }
  else if (mode == 1) {
    mode1();
  }
  else if (mode == 2) {
    mode2();
  }
}

void readBTSerial() {
  if (BTSerial.available()) {
    char input = char(BTSerial.read());

    if (input != '\r') {
      btString.concat(input);
    }

    if (input == '\n') {
      Serial.println("Ready");
      Serial.println(btString);

      // // Get action
      String action = getValue(btString, ',', 0);

      // Color
      if (action == "C") {
        changeColor(btString);
      }

      // Mode
      if (action == "M") {
        changeMode(btString);
      }

      btString = "";
    }
  }
}

void setup() {
  // Initialize monitor
  Serial.begin(9600);
  Serial.write("Interface série initalisée");

  // Initialize Bluetooth monitor
  BTSerial.begin(115200);

  // Initialize ambiant LEDs strip
  ambiantStrip.begin();
  ambiantStrip.setBrightness(100);
  ambiantStrip.fill(0, 0, 0);

  // Initialize
  seaStrip.begin();
  seaStrip.setBrightness(50);
  seaStrip.show();

  bridgeBottomStrip.begin();
  bridgeBottomStrip.setBrightness(50);
  bridgeBottomStrip.show();

  bridgeTopStrip.begin();
  bridgeTopStrip.setBrightness(80);
  bridgeTopStrip.show();

  towersLeftStrip.begin();
  towersLeftStrip.setBrightness(50);
  towersLeftStrip.show();

  towersRightStrip.begin();
  towersRightStrip.setBrightness(50);
  towersRightStrip.show();

  animatedSeaStrip.begin();
  animatedBridgeBottomStrip.begin();
  animatedBridgeTopStrip.begin();

  randomSeed(analogRead(0));
  int rand = random(3);

  mode1();

  // if (rand == 0) {
  //   mode0();
  // } else if (rand == 1) {
  //   mode1();
  // } else {
  //   mode2();
  // }
}

void loop() {
  // readBTSerial();

  Serial.println("o");

  // // do other stuff here like testing digital input (button presses) ...
  // animatedSeaStrip.update();
  // animatedBridgeBottomStrip.update();
  // animatedBridgeTopStrip.update();

  Adafruit_NeoPixel strip;



  for (int i=0; i<5; i++) {
    states_t state = states[i];

    if (i == 0) {
      strip = seaStrip;
    } else if (i == 1) {
      strip = bridgeBottomStrip;
    } else if (i == 2) {
      strip = bridgeTopStrip;
    } else if (i == 3) {
      strip = towersLeftStrip;
    } else {
      strip = towersRightStrip;
    }

    Serial.println(theaterChasePreviousMillis[i]);

    switch (state)
    {
      case FIXED:
        // Nothing
        break;

      case THEATER_CHASE:
        if (millis() - theaterChasePreviousMillis[i] >= pixelsInterval) {
          theaterChasePreviousMillis[i] = millis();
          theaterChase(i, strip, strip.Color(127, 127, 127)); // White
        }
        break;

      // case THEATER_CHASE_RAINBOW:
      //   if (millis() - theaterChaseRainbowPreviousMillis >= pixelsInterval) {
      //     theaterChaseRainbowPreviousMillis = millis();
      //     AnimatedStrip::theaterChaseRainbow();
      //   }
      //   break;

      // case RAINBOW:
      //   if (millis() - rainbowPreviousMillis >= pixelsInterval) {
      //     rainbowPreviousMillis = millis();
      //     AnimatedStrip::rainbow();
      //   }
      //   break;

      case RAINBOW_CYCLES:
        if (millis() - rainbowCyclesPreviousMillis[i] >= pixelsInterval) {
          rainbowCyclesPreviousMillis[i] = millis();
          rainbowCycle(i, strip);
        }
        break;
    }
  }
}