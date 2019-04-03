void setup() {
  Serial.begin(9600);
  pinMode(D1, OUTPUT);
}

void loop() {
  digitalWrite(D1, HIGH);
  delayMicroseconds(500);
  int a = analogRead(A0);
  Serial.println(a);
}
