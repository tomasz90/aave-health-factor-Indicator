#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>
#include <bitset>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

WiFiServer server(80);
WiFiManager manager;

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

  manager.setConnectTimeout(10);
  manager.setConfigPortalTimeout(120);
  manager.autoConnect("ESP_AAVE", "password");

  connectToWifi();
  server.begin();
}

void loop() {
  displayAaveColors();

  WiFiClient client = server.available();
  displayWebPage(client);
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

void connectToWifi() {
  int _try = 0;
  long lastTry = millis();
  long _delay = 10000;
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastTry > _delay) {
      WiFi.reconnect();
      lastTry = millis();
      _try++;
    }
    if(_try > 10) {
      ESP.restart();
    }
    blinkBlue();
  }
}
