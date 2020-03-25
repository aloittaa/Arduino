#include "oled.h"
#include <MyConfig.h> // credentials, servers, ports
#define MQTT_TOPIC "lights/WS2812B-8"
#define MQTT_TOPIC_CO2 "sensors/mh-z19b"
#define CO2_min 400  // for mapping leds/colors
#define CO2_max 3000 // seldomly goes that high; sensor goes up to 5000
#define MQTT_TOPIC_BME280 "sensors/bme280"
#include "wifi_ota_mqtt.h"
#include <ArduinoJson.h>

#include <FastLED.h>

// WS2812B 8x1 panel https://www.aliexpress.com/item/32897581470.html
// based on FastLED example DemoReel100.ino

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    D1
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    8
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96 // 0-255
#define FRAMES_PER_SECOND  120

byte mode = '0';
CRGB color;
unsigned short co2;
char co2_str[4];
char bme280_str[10];

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("MQTT message on topic %s with payload ", topic);
  for(int i = 0; i < length; i++){
    Serial.print((char) *(payload+i));
  }
  Serial.println();
  if (strcmp(topic, MQTT_TOPIC_CO2) == 0) {
    StaticJsonDocument<120> doc; // size 107 determined with https://arduinojson.org/v6/assistant/
    deserializeJson(doc, payload);
    co2 = doc["co2"];
    sprintf(co2_str, "%d", co2);
    // Serial.printf("%d -> %d\n", co2, map(co2, 400, 3000, 0, 7));
  } else if (strcmp(topic, MQTT_TOPIC_BME280) == 0) {
    StaticJsonDocument<80> doc; // size 78
    deserializeJson(doc, payload);
    sprintf(bme280_str, "%.1fC %d%%", (float)doc["temperature"], (int)doc["humidity"]);
  } else if (strcmp(topic, MQTT_TOPIC) == 0) {
    clear_OLED(); // clear display, otherwise output from previous mode would stay
    mode = *payload; // - '0';
    if (mode == '#') { // convert string hex color to int
      char c[6]; for(int i = 0; i<6; i++) c[i] = (char) *(payload+1+i); // need to convert (unsigned) byte to (signed) char
      unsigned long ul = strtoul(c, 0, 16); // would give wrong values for (char*)payload+1 since the content is unsigned
      Serial.println(ul, HEX);
      color = ul;
    } else if (mode == 'c') {
      mqtt.subscribe(MQTT_TOPIC_CO2);
      mqtt.subscribe(MQTT_TOPIC_BME280);
    } else {
      mqtt.unsubscribe(MQTT_TOPIC_CO2);
      mqtt.unsubscribe(MQTT_TOPIC_BME280);
    }
  }
}

void setup() {
  Serial.begin(38400);
  Serial.println("setup");
  setup_OLED();
  setup_WiFi();
  // setup_OTA();
  mqtt.setCallback(mqtt_callback);
  setup_MQTT();
  
  delay(2000); // 2 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void fill(CRGB color, int n = NUM_LEDS) {
  fill_solid(leds, n, color);
}

void loop() {
  // ArduinoOTA.handle();
  mqtt.loop();
  switch (mode) {
    case '0':
      fill(CRGB::Black);
      break;
    case '#':
      fill(color);
      // OLED.print(color);
      break;
    case 'c': // co2
      OLED.clearBuffer();
      // fonts: https://github.com/olikraus/u8g2/wiki/fntlistall
      OLED.setFont(u8g2_font_inb38_mf); // u8g2_font_logisoso32_tf
      OLED.drawStr(0, 38, co2_str);
      OLED.setFont(u8g2_font_crox5hb_tr);
      OLED.drawStr(0, 64, bme280_str);
      OLED.sendBuffer(); // if we do this outside in every loop, the FastLED demo becomes much slower!
      fill_gradient_RGB(leds, 0, CRGB::Green, 7, CRGB::Red); // all leds green to red
      { // need a block to be able to declare variables inside a switch-case
      byte led = map(co2, CO2_min, CO2_max, 0, 7); // single led to indicate current level
      CRGB color = leds[led]; // backup its gradient color
      fill(CRGB::Black); // turn off all leds
      leds[led] = color; // turn back on a single led with the gradient color
      leds[led].fadeLightBy(map(co2, CO2_min, CO2_max, 255, 0)); // dim lower levels (CO2_min is 100% dimmed w/o turning it off)
      }
      break;
    case 'd': // demo
    default:
      // OLED.print("demo");
      gPatterns[gCurrentPatternNumber](); // Call the current pattern function once, updating the 'leds' array
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for(int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for(int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
