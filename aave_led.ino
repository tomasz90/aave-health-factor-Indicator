#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <bitset>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

WiFiServer server(80);
String header;

struct Color {
  uint8 red;
  uint8 green;
  uint8 blue;
};

struct Settings {
  char chain[15];
  char address[42];
};

Settings settings;

String avaxRpc = "https://api.avax.network/ext/bc/C/rpc";
String polygonRpc = "https://polygon-rpc.com/";

String avaxPool = "0x4f01aed16d97e3ab5ab2b501154dc9bb0f1a5a2c";
String polygonPool = "0x8dFf5E27EA6b7AC08EbFdf9eB090F32ee9a30fcf";

String avaxPoolV3 = "0x794a61358D6845594F94dc1DB02A252b5b4814aD";
String polygonPoolV3 = "0x794a61358D6845594F94dc1DB02A252b5b4814aD";

unsigned long previousMillis = -25000;
const long interval = 30000;

static float hf;

void setup() {

  Serial.begin(9600);
  EEPROM.begin(sizeof(Settings));
  EEPROM.get(0, settings);

  setupAaveColors();
  setupIndicatorColors();

  WiFi.begin(ssid, password);
  connectToWifi();
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
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
            if (header.indexOf("GET /AvalancheV3") >= 0) {
              updateChain("AvalancheV3");
              updateHf();
            } else if (header.indexOf("GET /PolygonV3") >= 0) {
              updateChain("PolygonV3");
              updateHf();
            } else if (header.indexOf("GET /Avalanche") >= 0) {
              updateChain("Avalanche");
              updateHf();
            } else if (header.indexOf("GET /Polygon") >= 0) {
              updateChain("Polygon");
              updateHf();
            } else if (header.indexOf("GET /?address=") >= 0 && header.indexOf("0x") >= 0) {
              int firstIndex = header.indexOf("=") + 1;
              String address = header.substring(firstIndex);
              int lastIndex = address.indexOf(" ");
              char tmpa[42];
              address.substring(2, lastIndex).toCharArray(tmpa, 42);
              updateAddress(tmpa);
              updateHf();
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            // drop down styling
            client.println(".dropbtn {background-color: #04AA6D; min-width: 400px; color: white; padding: 16px; font-size: 20px; border: none; }");
            client.println(".dropdown { position: relative; display: inline-block; }");
            client.println(".dropdown-content { display: none; position: absolute; background-color: #f1f1f1; min-width: 400px; box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2); z-index: 1; }");
            client.println(".dropdown-content a { color: black; padding: 12px 16px; text-decoration: none; display: block; }");
            client.println(".dropdown-content a:hover {background-color: #ddd;}");
            client.println(".dropdown:hover .dropdown-content {display: block;}");
            client.println(".dropdown:hover .dropbtn {background-color: #3e8e41;}");
            // input text styling
            client.println("input[type=text] {   width: 400px;   padding: 12px 20px;   margin: 8px 0;   box-sizing: border-box;   border: 2px solid black;   border-radius: 4px; }");
            // button styling
            client.println(".button { background-color: #195B6A; width: 400px; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 22px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");

            // Web Page Heading
            client.println("<body><h1>AAVE indicator</h1>");
            String healthFactor = (hf < 4722) ? String(hf) : "&infin;";
            client.println("<h2>Health factor: " + healthFactor + "</h2>");
            // drop down menu
            client.println("<div class=\"dropdown\"> <button class=\"dropbtn\">" + String(settings.chain) + "</button> <div class=\"dropdown-content\">");
            client.println("<a href=\"Avalanche\">Avalanche</a> <a href=\"Polygon\">Polygon</a> <a href=\"AvalancheV3\">AvalancheV3</a> <a href=\"PolygonV3\">PolygonV3</a></div> </div>");
            // input and submit form
            client.println("<form action=\"/\" id=\"addressForm\"> <input type=\"text\" name=\"address\" placeholder=0x" + String(settings.address) + ">");
            client.println("<p><input type=\"submit\" class=\"button\" value=\"Submit\"> </p> </form>");
            // script to enter address
            client.println("<script> $(\"#addressForm\").submit(function(event) { event.preventDefault();");
            client.println("var $form = $(this), term = $form.find(\"input[name='address']\").val(), url = $form.attr(\"action\"); var posting = $.post(url, {address: term}); }); </script>");
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

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    updateHf();
  }
}

void updateHf() {
  connectToWifi(); // if disconnected
  if (strcmp(settings.chain, "AvalancheV3") == 0) {
    hf = getHf(avaxRpc, avaxPoolV3, settings.address);
  } else if (strcmp(settings.chain, "PolygonV3") == 0) {
    hf = getHf(polygonRpc, polygonPoolV3, settings.address);
  } else if (strcmp(settings.chain, "Avalanche") == 0) {
    hf = getHf(avaxRpc, avaxPool, settings.address);
  } else if (strcmp(settings.chain, "Polygon") == 0) {
    hf = getHf(polygonRpc, polygonPool, settings.address);
  }

  indicatorDisplay(hf);
  Serial.println(hf);
  Serial.println(settings.address);
  Serial.println(settings.chain);
}

float getHf(String rpc, String pool, String address) {
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    https.begin(*client, rpc);
    https.setTimeout(5000);
    https.addHeader("Content-Type", "application/json");
    String payload = "{\"method\":\"eth_call\",\"params\":[{\"to\":\"POOL\",\"data\":\"0xbf92857c000000000000000000000000ADDRESS\"},\"latest\"],\"id\":42,\"jsonrpc\":\"2.0\"}";
    payload.replace("POOL", pool);
    payload.replace("ADDRESS", address);
    float result = 0.0;
    int attempt = 0;

    while (attempt < 5 && result == 0.0) {
      int httpResponseCode = https.POST(payload);
      if (httpResponseCode == 200) {
        String response = https.getString();
        String hexNumber = response.substring(response.length() - 20, response.length() - 2);

        String truncated = hexNumber.substring(0, hexNumber.length() - 10); // truncated by 16^10
        float f = 0.0000010995;
        result = strtoul(truncated.c_str(), 0, 16) * f;
      }
      else {
        Serial.println("Error code: ");
        Serial.println(httpResponseCode);
        delay(10000);
      }
      attempt++;
    }
    https.end();
    return result;
  }
}

void updateChain(char* chain) {
  for (int i = 0; i < sizeof(settings.chain); i++) {
    settings.chain[i] = chain[i];
  }
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void updateAddress(char* address) {
  for (int i = 0; i < sizeof(settings.address); i++) {
    settings.address[i] = address[i];
  }
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void connectToWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    setBlue();
    delay(250);
    setBlank();
    delay(250);
    Serial.print(".");
  }
}
