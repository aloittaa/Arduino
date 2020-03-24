// 128x64 OLED display SSD1306
// 0.96" works, i2c_scanner: 0x3C
// 1.3" stays black but found by i2c_scanner: 0x3C
// probably broken since the glass is cracked in both lower corners, see https://github.com/ThingPulse/esp8266-oled-ssd1306/issues/179#issuecomment-453245963
// also tried U8g2lib instead of Adafruit_SSD1306 since the 1.3" uses the SSH1106 controller, but really seems to be broken.
// back to 0.96"; wanted to use Adafruit_SSD1306, but it only works with the default I2C pins; changing them via Wire.begin(D7, D6) does not work.
// -> use U8g2lib instead
#include <U8g2lib.h>
// U8G2_R2 = 180 degree clockwise rotation; change I2C pins, default on Wemos D1 mini: SCL:D1, SDA:D2
U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED(U8G2_R2, /* clock=*/ D6, /* data=*/ D7, /* reset=*/ U8X8_PIN_NONE);

// #define OLED_stat(...) OLED.clearDisplay(); OLED.setCursor(0, 0); OLED.setTextSize(2); OLED.printf(__VA_ARGS__); OLED.display(); OLED.setTextSize(1)
#define OLED_stat(...) OLED.clearBuffer(); OLED.drawStr(0, 10, __VA_ARGS__); OLED.sendBuffer()

void setup_OLED() {
  OLED.begin();
  
  OLED.clearBuffer();
  // fonts: https://github.com/olikraus/u8g2/wiki/fntlistall
  OLED.setFont(u8g2_font_inb38_mf); // u8g2_font_logisoso32_tf
  OLED.drawStr(0, 38, "2314");
  OLED.setFont(u8g2_font_crox5hb_tr);
  OLED.drawStr(0, 64, "-1  23.0 55%");
  OLED.sendBuffer();
}

void drawSymbols() {
  OLED.setFont(u8g2_font_unifont_t_symbols); // https://github.com/olikraus/u8g2/wiki/fntgrpunifont
  OLED.drawGlyph(0, 20, 9200); // alarm clock
  OLED.drawGlyph(20, 20, 9203); // hourglass
  OLED.drawGlyph(0, 40, 9731); // snowman
  OLED.drawGlyph(20, 40, 9749); // coffee
  OLED.setFont(u8g2_font_unifont_t_76);
  OLED.drawGlyph(0, 60, 9762); // radioactive
  OLED.setFont(u8g2_font_unifont_t_77);
  OLED.drawGlyph(20, 60, 9883); // atom; 9959: satanic pentagram
  OLED.setFont(u8g2_font_open_iconic_embedded_4x_t);
  OLED.drawGlyph(40, 32, 66); // gear
  OLED.drawGlyph(80, 32, 68); // home
  OLED.drawGlyph(40, 64, 70); // pulse
  OLED.setFont(u8g2_font_open_iconic_weather_4x_t);
  OLED.drawGlyph(80, 64, 67); // rain
}
