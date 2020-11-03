#include <Arduino.h>
#include <RgbMosfetStrip.h>
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
#define BRIDGE_BOTTOM_PIXELS 37
#define BRIDGE_TOP_PIXELS 12
#define TOWERS_LEFT_PIXELS 1
#define TOWERS_RIGHT_PIXELS 4

RgbMosfetStrip ambiantStrip = RgbMosfetStrip(RED_LED, GREEN_LED, BLUE_LED);
Adafruit_NeoPixel seaStrip = Adafruit_NeoPixel(SEA_PIXELS, SEA_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeBottomStrip = Adafruit_NeoPixel(BRIDGE_BOTTOM_PIXELS, BRIDGE_BOTTOM_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeTopStrip = Adafruit_NeoPixel(BRIDGE_TOP_PIXELS, BRIDGE_TOP_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersLeftStrip = Adafruit_NeoPixel(TOWERS_LEFT_PIXELS, TOWERS_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersRightStrip = Adafruit_NeoPixel(TOWERS_RIGHT_PIXELS, TOWERS_RIGHT, NEO_GRB + NEO_KHZ800);

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
  }
}

void changeMode(String inputString) {
  // TODO
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void readBTSerial() {
  if (BTSerial.available()) {
    String inputString = BTSerial.readString();

    // Get action
    String action = getValue(inputString, ',', 0);

    // Color
    if (action == "C") {
      changeColor(inputString);
    }

    // Mode
    if (action == "M") {
      changeMode(inputString);
    }
  }
}

void setup() {
  // Initialize monitor
  Serial.begin(9600);

  // Initialize Bluetooth monitor
  BTSerial.begin(115200);

  // Initialize ambiant LEDs strip
  ambiantStrip.begin();
  ambiantStrip.setBrightness(100);
  ambiantStrip.fill(0, 127, 127);

  // Initialize
  seaStrip.begin();
  seaStrip.setBrightness(20);
  seaStrip.fill(seaStrip.Color(0, 127, 255), 0, SEA_PIXELS);
  seaStrip.show();

  bridgeBottomStrip.begin();
  bridgeBottomStrip.setBrightness(50);
  bridgeBottomStrip.fill(bridgeBottomStrip.Color(127, 127, 127), 0, BRIDGE_BOTTOM_PIXELS);
  bridgeBottomStrip.show();

  bridgeTopStrip.begin();
  bridgeTopStrip.setBrightness(80);
  bridgeTopStrip.fill(bridgeTopStrip.Color(127, 127, 127), 0, BRIDGE_TOP_PIXELS);
  bridgeTopStrip.show();

  towersLeftStrip.begin();
  towersLeftStrip.setBrightness(30);
  towersLeftStrip.fill(towersLeftStrip.Color(0, 0, 255), 0, TOWERS_LEFT_PIXELS);
  towersLeftStrip.show();

  towersRightStrip.begin();
  towersRightStrip.setBrightness(30);
  towersRightStrip.fill(towersRightStrip.Color(255, 0, 0), 0, TOWERS_RIGHT_PIXELS);
  towersRightStrip.show();
}

void loop() {
  readBTSerial();

  // ambiantStrip.fill(255, 0, 0);
  // delay(5000);
  // ambiantStrip.fill(0, 255, 0);
  // delay(5000);
  // ambiantStrip.fill(0, 0, 255);
  // delay(5000);
}