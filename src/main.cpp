#include <Arduino.h>
#include "Bme280.hpp"
#include "GroveMP503.hpp"
#include "Lcd.hpp"

const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int SIGNAL_PIN = 35; 

Bme280 bme280;
GroveMP503 groveMp503;
Lcd lcd;

void setup() {
  Serial.begin(9600);
  delay(500);

  bme280.begin(SDA_PIN, SCL_PIN);
  groveMp503.begin(SIGNAL_PIN);
  lcd.begin();

}

void loop() {
  // Pobierz dane z czujników
  Bme280Data envData = bme280.readValues();
  GroveMP503Data airData = groveMp503.readAirQuality();

  // Wypisz dane na Serial
  Serial.print("Temperatura: "); Serial.print(envData.temperature); Serial.println(" °C");
  Serial.print("Ciśnienie: "); Serial.print(envData.pressure); Serial.println(" hPa");
  Serial.print("Wilgotność: "); Serial.print(envData.humidity); Serial.println(" %");
  Serial.print("Jakość powietrza: "); Serial.println(airData.quality.c_str());
  Serial.println("------------------------");

  // Wyświetl dane na LCD
  lcd.displayData(envData.temperature, envData.humidity, envData.pressure, String(airData.quality.c_str()));

  delay(3000);
}
