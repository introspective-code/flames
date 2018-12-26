#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <PubSubClient.h>

const char *ssid = "<SSID>";
const char *password = "<PASSWORD>";

const char *mqttServer = "<MQTTSERVER>";
const int mqttPort = 1883;

#define NUM_LEDS 111
#define DATA_PIN D4

CRGB leds[NUM_LEDS];

WiFiClient espClient;
PubSubClient client(espClient);

bool flamesAreOn = false;

void callback(char *topic, byte *payloadRaw, unsigned int length) {
  payloadRaw[length] = '\0';
  String payload = String((char *)payloadRaw);
  Serial.println(payload);
  if (payload == "TURN FLAMES ON") {
    flamesAreOn = true;
  } else if (payload == "TURN FLAMES OFF") {
    flamesAreOn = false;
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

    if (client.connect("ESP8266Client")) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  client.subscribe("esp/test");
}

void loop() {
  client.loop();

  if (flamesAreOn) {
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
  } else {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  }

  FastLED.show();
}
