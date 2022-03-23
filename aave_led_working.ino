#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <bitset>

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

WiFiServer server(80);

String header;
String chain = "Avalanche";

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

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  setupAaveColors();
  setupIndicatorColors();
  //indicatorDisplay(12);
}

void loop() {
  displayAaveColors();

  WiFiClient client = server.available(); // Listen for incoming clients
  if (client) { // If a new client connects,
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // turns the GPIOs on and off
            if (header.indexOf("GET /green/on") >= 0) {
              Serial.println("green on");
              //greenstate = "on";
              //digitalWrite(greenled, HIGH);
            } else if (header.indexOf("GET /green/off") >= 0) {
              Serial.println("green off");
              //greenstate = "off";
              //digitalWrite(greenled, LOW);
            } else if (header.indexOf("GET /red/on") >= 0) {
              Serial.println("red on");
              //redstate = "on";
              //digitalWrite(redled, HIGH);
            } else if (header.indexOf("GET /red/off") >= 0) {
              Serial.println("red off");
              //redstate = "off";
              //digitalWrite(redled, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            // drop down
            client.println(".dropbtn {background-color: #04AA6D; min-width: 160px; color: white; padding: 16px; font-size: 16px; border: none; }");
            client.println(".dropdown { position: relative; display: inline-block; }");
            client.println(".dropdown-content { display: none; position: absolute; background-color: #f1f1f1; min-width: 160px; box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2); z-index: 1; }");
            client.println(".dropdown-content a { color: black; padding: 12px 16px; text-decoration: none; display: block; }");
            client.println(".dropdown-content a:hover {background-color: #ddd;}");
            client.println(".dropdown:hover .dropdown-content {display: block;}");
            client.println(".dropdown:hover .dropbtn {background-color: #3e8e41;}");
            client.println("</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>AAVE indicator</h1>");
            
            client.println("<div class=\"dropdown\"> <button class=\"dropbtn\">" + chain + "</button> <div class=\"dropdown-content\">");
            client.println("<a href=\"#\">Avalanche</a> <a href=\"#\">Polygon</a> <a href=\"#\">Ethereum</a> </div> </div>");
           
            // Display current state, and ON/OFF buttons for GPIO 4
            //client.println("<p>red - State " + redstate + "</p>");
            // If the red LED is off, it displays the ON button
            if ("" == "off") {
              client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/red/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void indicatorDisplay(int pixels) {
  for (int i = 0; i < pixels; i++) {
    indicator.setPixelColor(i, indicator_colors[i]);
  }
  indicator.show();
}

void setupIndicatorColors() {
  indicator.begin();
  indicator.setBrightness(10);
}
void setupAaveColors() {
  pixels.begin();
  pixels.setBrightness(80);
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
  colors[0] = colors[6];
  colors[6] = colors[5];
  colors[5] = colors[4];
  colors[4] = colors[3];
  colors[3] = colors[2];
  colors[2] = colors[1];
  colors[1] = colors[0];
}
