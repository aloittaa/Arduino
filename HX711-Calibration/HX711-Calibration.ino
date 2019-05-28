/*
   https://www.hackster.io/MOHAN_CHANDALURU/hx711-load-cell-amplifier-interface-with-arduino-fa47f3

  Arduino
  2 -> HX711 CLK
  3 -> DOUT
  5V -> VCC
  GND -> GND

  Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
  The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/

// https://github.com/bogde/HX711
#include "HX711.h"  //You must have this library in your arduino library folder

#define DOUT  3
#define CLK  2

HX711 scale(DOUT, CLK);

float calibration_factor = -2000; // 5kg load cell: -402750

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");
  scale.set_scale();
  scale.tare(); // reset the scale to 0

  long zero_factor = scale.read_average(); // get a baseline reading
  Serial.print("Zero factor: "); // this can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
  scale.set_scale(calibration_factor);
  Serial.print("Reading: ");
  Serial.print(scale.get_units(10) * 1000, 3);
  Serial.print(" g");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if (Serial.available()) {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if (temp == 's')
      calibration_factor += 100;
    else if (temp == 'x')
      calibration_factor -= 100;
    else if (temp == 'd')
      calibration_factor += 1000;
    else if (temp == 'c')
      calibration_factor -= 1000;
    else if (temp == 'f')
      calibration_factor += 10000;
    else if (temp == 'v')
      calibration_factor -= 10000;
    else if (temp == 't') {
      scale.tare();  // reset the scale to zero
      long zero_factor = scale.read_average(); // get a baseline reading
      Serial.print("Zero factor: "); // this can be used to remove the need to tare the scale. Useful in permanent scale projects.
      Serial.println(zero_factor);
    }
  }
}
