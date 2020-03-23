// power consumption test (0.5s LED blink between phases)
// 5s LED on
// 5s LED off
// 5s Bluetooth on
// connect to WIFI, GET google.com, disconnect
// 30s deep sleep

// current in mA using 2 AA (~3.15 V)
// 1. measured with my multimeter (have to switch cables from mA to uA for deep sleep)
// 2. measured with Voltcraft VC275TRMS (has mA and uA on the same plug)
// tracked phases via serial (just RX,TX,GND), more brownouts with 2, maybe b/c of higher internal resistance
// LOLIN D32: ~38 work, 0.691 deep sleep
// 1. 39.4 , 39.2 , 42.3, 60-140, 0.3-0.8
// 2. 37.15, 36.84, ??, 691 uA, ? = 'Brownout detector was triggered' for BT or WiFi (68.7 constant current after)
// DOIT ESP32 DEVKIT V1: ~47 work, 9.7 deep sleep
// 2. 47.20, 48.85, brownout 80.0, 9.72 mA deep sleep
// 1. 47.5 , 48.0 , 52.2, <140, 9.1
// FireBeetle-ESP32 (bare ESP32 on white PCB): 38 work , 0.006 deep sleep
// 1. 37.7, 37.9, brownout 70, 6.63 uA deep sleep via 3V3 from USB
// 2. 37.79, ?, brownout 69.3, connected to USB, and got only some runs w/o brownouts:
// 2. 37.58, 37.59, 41.55, <136, 5.5 uA deep sleep

// also see https://docs.google.com/spreadsheets/d/1Mu-bNwpnkiNUiM7f2dx8-gPnIAFMibsC2hMlWhIHbPQ/edit#gid=0
// LOLIN D32 there says 125 uA deep sleep from battery

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
