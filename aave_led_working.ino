#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <bitset>

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

WiFiServer server(80);

#define AAVE_PIN 4
#define AAVE_NUMPIXELS 7
#define DELAYVAL 35

Adafruit_NeoPixel pixels(AAVE_NUMPIXELS, AAVE_PIN, NEO_GRB + NEO_KHZ800);

uint32_t color0 = pixels.Color(100, 160, 170);
uint32_t color1 = pixels.Color(140, 150, 150);
uint32_t color2 = pixels.Color(170, 130, 130);
uint32_t color3 = pixels.Color(200, 120, 110);
uint32_t color4 = pixels.Color(215, 95, 95);
uint32_t color5 = pixels.Color(230, 60, 80);
uint32_t color6 = pixels.Color(250, 30, 60);

uint32_t colors[7] = {color0, color1, color2, color3, color4, color5, color6};
bool setColor[7] = {false, false, false, false, false, false, false};

#define INDICATOR_PIN 13
#define INDICATOR_NUMPIXELS 16

Adafruit_NeoPixel indicator(INDICATOR_NUMPIXELS, INDICATOR_PIN, NEO_GRB + NEO_KHZ800);
uint32_t indicatorColor0 = indicator.Color(240, 20, 15);
uint32_t indicatorColor1 = indicator.Color(230, 30, 10);
uint32_t indicatorColor2 = indicator.Color(220, 40, 5);
uint32_t indicatorColor3 = indicator.Color(210, 50, 0);
uint32_t indicatorColor4 = indicator.Color(200, 60, 0);
uint32_t indicatorColor5 = indicator.Color(190, 70, 0);
uint32_t indicatorColor6 = indicator.Color(180, 80, 0);
uint32_t indicatorColor7 = indicator.Color(170, 90, 0);
uint32_t indicatorColor8 = indicator.Color(160, 100, 0);
uint32_t indicatorColor9 = indicator.Color(150, 110, 0);
uint32_t indicatorColor10 = indicator.Color(140, 120, 0);
uint32_t indicatorColor11 = indicator.Color(130, 130, 0);
uint32_t indicatorColor12 = indicator.Color(120, 140, 0);
uint32_t indicatorColor13 = indicator.Color(110, 150, 5);
uint32_t indicatorColor14 = indicator.Color(100, 160, 10);
uint32_t indicatorColor15 = indicator.Color(90, 170, 15);

uint32_t indicator_colors[16] = {
  indicatorColor0, indicatorColor1, indicatorColor2, indicatorColor3,
  indicatorColor4, indicatorColor5, indicatorColor6, indicatorColor7,
  indicatorColor8, indicatorColor9, indicatorColor10, indicatorColor11,
  indicatorColor12, indicatorColor13, indicatorColor14, indicatorColor15
};
struct Color {
  uint8 red;
  uint8 green;
  uint8 blue;
};

void setup() {

  Serial.begin(9600);

  //  WiFi.begin(ssid, password);
  //  Serial.print("Connecting to ");
  //  Serial.println(ssid);
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }

  setupAaveColors();
  indicator.begin();
  indicator.setBrightness(30);
  indicatorDisplay(16);
}

void loop() {
  displayAaveColors();
}

void indicatorDisplay(int pixels) {
  for (int i = 0; i < pixels; i++) {
     indicator.setPixelColor(i, indicator_colors[i]);
  }
  indicator.show();
}

void setupAaveColors() {
  pixels.begin();
  pixels.setBrightness(30);
  pixels.setPixelColor(0, colors[0]);
  pixels.setPixelColor(1, colors[1]);
  pixels.setPixelColor(2, colors[2]);
  pixels.setPixelColor(3, colors[3]);
  pixels.setPixelColor(4, colors[4]);
  pixels.setPixelColor(5, colors[5]);
  pixels.setPixelColor(6, colors[6]);
  pixels.show();

  changeDestinationColors();
}

void displayAaveColors() {
  bool allSet = true;

  for (int i = 0; i < AAVE_NUMPIXELS; i++) {
    Color destColor = binToRGB(colors[i]);
    setColor[i] = changeColors(i, destColor);
    if (!setColor[i]) {
      allSet = false;
    }
  }

  delay(DELAYVAL);

  if (allSet) {
    changeDestinationColors();
    for (int i = 0; i < sizeof(setColor); i++) {
      setColor[i] = false;
    }
  }
}

std::bitset<8> splitArray(std::bitset<32> binary, int first, int last) {
  std::bitset<8> newBitset;
  for (int n = first; n < last; n++) {
    newBitset[n - first] = binary[n];
  }
  return newBitset;
}

Color binToRGB(uint32_t color) {
  std::bitset<32> binary = std::bitset<32>(color);

  std::bitset<8> redBin = splitArray(binary, 16, 24);
  std::bitset<8> greenBin = splitArray(binary, 8, 16);
  std::bitset<8> blueBin = splitArray(binary, 0, 8);

  uint8 red = (int)redBin.to_ulong();
  uint8 green = (int)greenBin.to_ulong();
  uint8 blue = (int)blueBin.to_ulong();
  Color c = {red, green, blue};
  return c;
}

bool changeColors(int pixelNo, Color dest) {

  Color current = binToRGB(pixels.getPixelColor(pixelNo));
  uint8 divider = 16;
  uint8 red = findNextColor(current.red, dest.red, divider);
  uint8 green = findNextColor(current.green, dest.green, divider);
  uint8 blue = findNextColor(current.blue, dest.blue, divider);

  pixels.setPixelColor(pixelNo, pixels.Color(red, green, blue));
  pixels.show();

  bool result = red / divider == dest.red / divider && green / divider == dest.green / divider && blue / divider == dest.blue / divider;
  Serial.println(result);
  return result;
}

uint8 findNextColor(uint8 curr, uint8 dest, uint8 divider) {
  if (dest / divider - curr / divider > 0) {
    return curr + 12;
  } else if (dest / divider - curr / divider < 0) {
    return curr - 12;
  } else {
    return curr;
  }
}

void changeDestinationColors() {
  colors[6] = colors[0];
  colors[0] = colors[1];
  colors[1] = colors[2];
  colors[2] = colors[3];
  colors[3] = colors[4];
  colors[4] = colors[5];
  colors[5] = colors[6];
}
