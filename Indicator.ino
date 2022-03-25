#define INDICATOR_PIN 13
#define INDICATOR_NUMPIXELS 16
#define BUZZER_PIN 14

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
uint32_t indicatorColorX = indicator.Color(0, 0, 0);

uint32_t indicator_colors[16] = {
  indicatorColor0, indicatorColor1, indicatorColor2, indicatorColor3,
  indicatorColor4, indicatorColor5, indicatorColor6, indicatorColor7,
  indicatorColor8, indicatorColor9, indicatorColor10, indicatorColor11,
  indicatorColor12, indicatorColor13, indicatorColor14, indicatorColor15
};

void setupIndicatorColors() {
  indicator.begin();
  indicator.clear();
  indicator.setBrightness(15);
  pinMode(BUZZER_PIN, OUTPUT);
}

void indicatorDisplay(float hf) {
  if (hf != 0.0) {
    int pixels = (hf - 1.5) / 0.09;
    int firstBlack = 0;

    while (indicator.getPixelColor(firstBlack) != 0) {
      firstBlack++;
    }
    Serial.println(pixels);
    Serial.println(firstBlack);
    if (pixels >= 0) {
      if (firstBlack <= pixels) {
        if (pixels < 38) { // 38 -> hf = 5
          if(pixels > INDICATOR_NUMPIXELS - 1) {
            pixels = INDICATOR_NUMPIXELS - 1;
          }
          for (int i = 0; i <= pixels; i++) {
            indicator.setPixelColor(i, indicator_colors[i]);
            indicator.show();
            delay(200);
          }
        } else {
          setAllPixels(indicatorColor15);
        }
      } else if (firstBlack > pixels) {
        for (int i = firstBlack; i > pixels; i--) {
          indicator.setPixelColor(i, indicatorColorX);
          indicator.show();
          delay(200);
        }
      }
    } else {
      for (int i = 0; i < 10; i++) {
        tone(BUZZER_PIN, 1000);
        setAllPixels(indicatorColorX);
        delay(300);
        noTone(BUZZER_PIN);
        setAllPixels(indicatorColor0);
        delay(300);
      }
    }
  } else {
    for (int i = 0; i < 10; i++) {
      setAllPixels(indicatorColorX);
      delay(300);
      setAllPixels(indicatorColor7);
      delay(300);
    }
  }
}

void setAllPixels(int color) {
  for (int i = 0; i < INDICATOR_NUMPIXELS; i++) {
    indicator.setPixelColor(i, color);
  }
  indicator.show();
}
