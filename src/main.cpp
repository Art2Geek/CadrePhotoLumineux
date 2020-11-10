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

// how much serial data we expect before a newline
const unsigned int MAX_INPUT = 50;

RgbMosfetStrip ambiantStrip = RgbMosfetStrip(RED_LED, GREEN_LED, BLUE_LED);
Adafruit_NeoPixel seaStrip = Adafruit_NeoPixel(SEA_PIXELS, SEA_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeBottomStrip = Adafruit_NeoPixel(BRIDGE_BOTTOM_PIXELS, BRIDGE_BOTTOM_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bridgeTopStrip = Adafruit_NeoPixel(BRIDGE_TOP_PIXELS, BRIDGE_TOP_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersLeftStrip = Adafruit_NeoPixel(TOWERS_LEFT_PIXELS, TOWERS_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel towersRightStrip = Adafruit_NeoPixel(TOWERS_RIGHT_PIXELS, TOWERS_RIGHT, NEO_GRB + NEO_KHZ800);

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
  }
}

void mode0() {
  Serial.println("Mode 0");

  animatedSeaStrip.setFixedMode();
  animatedBridgeBottomStrip.setFixedMode();
  animatedBridgeTopStrip.setFixedMode();

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

  animatedSeaStrip.setRainbowCyclesMode();
  animatedBridgeBottomStrip.setTheaterChaseMode();
  animatedBridgeTopStrip.setTheaterChaseMode();

  towersLeftStrip.fill(towersLeftStrip.Color(0, 0, 255), 0, TOWERS_LEFT_PIXELS);
  towersLeftStrip.show();

  towersRightStrip.setPixelColor(0, towersRightStrip.Color(255, 0, 0));
  towersRightStrip.fill(towersRightStrip.Color(127, 127, 127), 1, 2);
  towersRightStrip.setPixelColor(3, towersRightStrip.Color(255, 0, 0));
  towersRightStrip.show();
}

void changeMode(String inputString) {
  int mode = getValue(inputString, ',', 1).toInt();

  // Serial.println("Change mode");

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
}

// here to process incoming serial data after a terminator received
void process_data (const char * data)
{
  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  Serial.println(data);

  // Get action
  String action = getValue(data, ',', 0);

  Serial.print("Action : ");
  Serial.println(action);

  // Color
  if (action == "C") {
    changeColor(data);
  }

  // Mode
  if (action == "M") {
    changeMode(data);
  }
}  // end of process_data

void processIncomingByte (const byte inByte)
{
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte)
  {

    case '\n':   // end of text
      input_line[input_pos] = 0;  // terminating null byte

      // terminator reached! process input_line here ...
      process_data(input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

    }  // end of switch

} // end of processIncomingByte

void setup() {
  // Initialize monitor
  Serial.begin(9600);
  delay(1000);
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

  mode1();
}

void loop() {
  // readBTSerial();

  while (BTSerial.available() > 0) {
    processIncomingByte(BTSerial.read());
  }

  // do other stuff here like testing digital input (button presses) ...
  animatedSeaStrip.update();
  animatedBridgeBottomStrip.update();
  animatedBridgeTopStrip.update();
}