#include <FastLED.h>

#define NUM_LEDS 111
#define DATA_PIN D4

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
  int r = 255;
  int g = r - 80;
  int b = 40;

  for (int i = 0; i < NUM_LEDS; i++) {
    int flicker = random(0, 150);
    int r1 = r - flicker;
    int g1 = g - flicker;
    int b1 = b - flicker;

    if(g1 < 0) g1 = 0;
    if(r1 < 0) r1 = 0;
    if(b1 < 0) b1 = 0;

    leds[i].red = r1;
    leds[i].green = g1;
    leds[i].blue = b1;
  }
  FastLED.show();
  delay(random(25,125));
}