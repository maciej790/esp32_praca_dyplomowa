#include <Arduino.h>
#include "Bme280.hpp"
#include "GroveMP503.hpp"

const int SDA_PIN = 22;
const int SCL_PIN = 23;
const int SIGNAL_PIN = 25;

Bme280 bme280;
GroveMP503 groveMp503;

void setup() {
  Serial.begin(9600);
  //bme280 init
  bme280.begin(SDA_PIN, SCL_PIN);
  //grovemp503 init
  groveMp503.begin(SIGNAL_PIN);
}

void loop() {
  bme280.readValuesFromSensor();
  groveMp503.readAirQualityValue();
  delay(2000);
}
