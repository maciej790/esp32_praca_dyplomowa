#include <Arduino.h>
#include <WiFi.h>
#include "Bme280.hpp"
#include "GroveMP503.hpp"
#include "Lcd.hpp"
#include "HttpSender.hpp"

const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int SIGNAL_PIN = 35;

const char *ssid = "vnet-894ED2";
const char *password = "001dd2894ed2";

// zmieniono adres IP na lokalny adres serwera
HttpSender sender("http://192.168.0.7:3000/dashboard/sensor_data");

Bme280 bme280;
GroveMP503 groveMp503;
Lcd lcd;

void setup()
{
  Serial.begin(9600);
  delay(500);

  bme280.begin(SDA_PIN, SCL_PIN);
  groveMp503.begin(SIGNAL_PIN);
  lcd.begin();
  sender.begin(ssid, password);

  pinMode(2, OUTPUT); // LED status
}

void loop()
{
  // Odczyt czujników
  Bme280Data envData = bme280.readValues();
  GroveMP503Data airData = groveMp503.readAirQuality();

  // Wyświetlenie w konsoli
  Serial.print("Temperatura: ");
  Serial.println(envData.temperature);
  Serial.print("Ciśnienie: ");
  Serial.println(envData.pressure);
  Serial.print("Wilgotność: ");
  Serial.println(envData.humidity);
  Serial.print("Napięcie: ");
  Serial.println(airData.voltage);
  Serial.print("Jakość powietrza: ");
  Serial.println(airData.quality.c_str());
  Serial.println("------------------------");

  // Wyświetlenie na LCD
  lcd.displayData(envData.temperature, envData.humidity, envData.pressure, String(airData.quality.c_str()));

  // Przygotowanie próbki
  SensorSample sample;
  sample.temperature = envData.temperature;
  sample.humidity = envData.humidity;
  sample.pressure = envData.pressure;
  sample.voltage = airData.voltage;
  sample.airQuality = String(airData.quality.c_str());

  // Dodanie próbki do bufora i ewentualna wysyłka
  sender.addSample(sample, ssid, password);

  // Status LED: świeci gdy WiFi połączone
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }

  delay(500); // np. co 0.5s dodajemy próbkę
}
