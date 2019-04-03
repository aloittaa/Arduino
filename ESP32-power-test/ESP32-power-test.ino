// power consumption test (0.5s LED blink between phases)
// 5s LED on
// 5s LED off
// 5s Bluetooth on
// connect to WIFI, GET google.com, disconnect
// 30s deep sleep

// current in mA using 2 AA (~3.15 V)
// LOLIN D32: 39.3-39.4, 39.2, 42.3, 60-140, 0.3-0.8

#include <MyConfig.h>

#include <WiFi.h>
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  pinMode(LED_BUILTIN, OUTPUT);
}

void setup_wifi() {
  delay(5);
  Serial.printf("Connecting to AP %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void http_get() {
  client.connect("google.com", 80);
  client.print("GET / HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n");
  Serial.println("GET / on google.com:80:");
  while(!client.available()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Response (each . one line):");
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(".");
  }
}

void blink(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  Serial.println("LED on for 5s");
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
  Serial.println("LED off for 5s");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  blink();
  Serial.println("BT on for 5s");
  btStart();
  delay(5000);
  btStop();
  Serial.println("BT off");
  blink();
  Serial.println("WIFI connect");
  setup_wifi();
  http_get();
  Serial.println("and disconnect");
  WiFi.disconnect();
  blink();
  Serial.println("30s deep sleep");
  esp_sleep_enable_timer_wakeup(30 * 1000 * 1000);
  esp_deep_sleep_start();
}
