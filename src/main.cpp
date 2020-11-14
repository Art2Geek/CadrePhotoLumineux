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

// AnimatedStrip animatedSeaStrip = AnimatedStrip(seaStrip);
// AnimatedStrip animatedBridgeBottomStrip = AnimatedStrip(bridgeBottomStrip);
// AnimatedStrip animatedBridgeTopStrip = AnimatedStrip(bridgeTopStrip);

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
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return seaStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return seaStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return seaStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

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

  for (int i=0; i<seaStrip.numPixels(); i++) {
    seaStrip.setPixelColor(i, Wheel(((i * 256 / seaStrip.numPixels())) & 255));
  }
  seaStrip.show();

  bridgeTopStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_TOP_PIXELS);
  bridgeTopStrip.show();

  bridgeBottomStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_BOTTOM_PIXELS);
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

  // animatedSeaStrip.begin();
  // animatedBridgeBottomStrip.begin();
  // animatedBridgeTopStrip.begin();

  randomSeed(analogRead(0));
  int rand = random(3);

  if (rand == 0) {
    mode0();
  } else if (rand == 1) {
    mode1();
  } else {
    mode2();
  }
}

void loop() {
  readBTSerial();

  // // do other stuff here like testing digital input (button presses) ...
  // animatedSeaStrip.update();
  // animatedBridgeBottomStrip.update();
  // animatedBridgeTopStrip.update();
}