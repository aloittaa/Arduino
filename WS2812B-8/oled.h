// 128x64 OLED display SSD1306
// 0.96" works, i2c_scanner: 0x3C
// 1.3" stays black but found by i2c_scanner: 0x3C
// probably broken since the glass is cracked in both lower corners, see https://github.com/ThingPulse/esp8266-oled-ssd1306/issues/179#issuecomment-453245963
// also tried U8g2lib instead of Adafruit_SSD1306 since the 1.3" uses the SSH1106 controller, but really seems to be broken.
// back to 0.96"; wanted to use Adafruit_SSD1306, but it only works with the default I2C pins; changing them via Wire.begin(D7, D6) does not work.
// -> use U8g2lib instead
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
Adafruit_SSD1306 OLED(128, 64, &Wire, 0);

#define OLED_stat(...) OLED.clearDisplay(); OLED.setCursor(0, 0); OLED.setTextSize(2); OLED.printf(__VA_ARGS__); OLED.display(); OLED.setTextSize(1)

void setup_OLED() {
  Wire.begin(D7, D6); // change from default I2C pins on Wemos D1 mini: SDA:D2, SCL:D1
  OLED.begin();
  OLED.setFont(&FreeSans9pt7b);
  OLED.setTextColor(WHITE);
  OLED.clearDisplay(); OLED.display(); // clear to avoid noise
  OLED.println("hallo");
  OLED.display();
}
