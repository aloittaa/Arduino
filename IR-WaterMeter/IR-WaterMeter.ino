// WAVGAT UNO R3 differences: http://forum.arduino.cc/index.php?topic=560692.0

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  Serial.println("init");
}

// original UNO R3 accepts 5V analog input (0-1023)
// WAVGAT: analogical inputs go from 0.0V (0) to 3.3V (4064) [ADC 12 bits]
// then decreases up to 5.0V (2540)
// the following didn't change anything, see https://forum.arduino.cc/index.php?topic=540421.msg4025095#msg4025095
void setADC() {
  analogReference(EXTERNAL); // means AREF is used as a comparison -> connect to 5V?
  analogReadResolution(12);
}

void loop() {
//  digitalWrite(8, HIGH); // GPIOs should be 5V but are 3.3V
  delayMicroseconds(500);
  // TCRT5000: IR RX 3.3V, IR TX 3.3V (via GPIO 8)
  // finger: 4064 (>5.5cm) to 213 (0cm)
  // big sep. IR LEDs: IR RX 3.3V, IR TX 3.3V (via GPIO 8)
  // finger: 3843 (>?) to 322 (0cm)
  // big sep. IR LEDs: IR RX 3.3V, IR TX 5V
  // finger: 3497 (>?) to 319 (0cm)
  // NB: can't use A4 & A5 if using I2C
  Serial.print(analogRead(A0));
  Serial.print("\t");
  Serial.print(analogRead(A1));
  Serial.println();
}
