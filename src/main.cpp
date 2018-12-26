#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <PubSubClient.h>

#include "config.h"

#define NUM_LEDS 111
#define DATA_PIN D4

CRGB leds[NUM_LEDS];

WiFiClient espClient;
PubSubClient client(espClient);

enum Effects { Flames };
enum SwitchState { On, Off };

Effects effect = Flames;
SwitchState switchState = Off;

void callback(char *topic, byte *payloadRaw, unsigned int length) {
  payloadRaw[length] = '\0';
  String payload = String((char *)payloadRaw);

  if (payload == "Effect: Flames") {
    effect = Flames;
  } else if (payload == "Switch: On") {
    switchState = On;
  } else if (payload == "Switch: Off") {
    switchState = Off;
  }
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("Headboard Flames")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  client.subscribe("homie/headboard_flames/strip/effect");
  client.subscribe("homie/headboard_flames/strip/switch");
}

void stripSwitchOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void flameEffect() {
  int r = 255;
  int g = r - 80;
  int b = 40;

  for (int i = 0; i < NUM_LEDS; i++) {
    int flicker = random(0, 150);
    int r1 = r - flicker;
    int g1 = g - flicker;
    int b1 = b - flicker;

    if (g1 < 0)
      g1 = 0;
    if (r1 < 0)
      r1 = 0;
    if (b1 < 0)
      b1 = 0;

    leds[i].red = r1;
    leds[i].green = g1;
    leds[i].blue = b1;
  }
  delay(random(25, 125));
}

void loop() {
  client.loop();

  if (switchState == Off)
    return stripSwitchOff();

  switch (effect) {
  case Flames:
    flameEffect();
    break;
  }

  FastLED.show();
}
