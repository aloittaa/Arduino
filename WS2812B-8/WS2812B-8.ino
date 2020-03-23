#include <MyConfig.h> // credentials, servers, ports
#define MQTT_TOPIC "lights/WS2812B-8"
#include "wifi_ota_mqtt.h"

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

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

byte mode = '0';
CRGB color;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("MQTT message on topic %s with payload ", topic);
  for(int i = 0; i < length; i++){
    Serial.print((char) *(payload+i));
  }
  Serial.println();
  mode = *payload; // - '0';
  if (mode == '#') { // convert string hex color to int
    char c[6]; for(int i = 0; i<6; i++) c[i] = (char) *(payload+1+i); // need to convert (unsigned) byte to (signed) char
    unsigned long ul = strtoul(c, 0, 16); // would give wrong values for (char*)payload+1 since the content is unsigned
    Serial.println(ul, HEX);
    color = ul;
  }
}

void setup() {
  Serial.begin(38400);
  Serial.println("setup");
  setup_wifi();
  setup_OTA();
  mqtt.setCallback(mqtt_callback);
  setup_mqtt();
  
  delay(2000); // 2 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  ArduinoOTA.handle();
  mqtt.loop();
  switch (mode) {
    case '0':
      // FastLED.clear();
      fill(CRGB::Black);
      break;
    case '#':
      fill(color);
      break;
    default:
      // Call the current pattern function once, updating the 'leds' array
      gPatterns[gCurrentPatternNumber]();
      // send the 'leds' array out to the actual LED strip
      FastLED.show();  
      // insert a delay to keep the framerate modest
      FastLED.delay(1000/FRAMES_PER_SECOND); 
      // do some periodic updates
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
}

void fill(CRGB color) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
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
