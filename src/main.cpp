#include <Arduino.h>
#include <WiFi.h>
#include "Bme280.hpp"
#include "GroveMP503.hpp"
#include "Lcd.hpp"
#include "HttpSender.hpp"

// 🧩 Piny I2C i czujników
const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int SIGNAL_PIN = 35;

// 🌐 WiFi
const char *ssid = "vnet-894ED2";
const char *password = "001dd2894ed2";

// 🌍 Adres lokalnego serwera
HttpSender sender("http://192.168.0.7:3000/sensor_data");

// 🧠 Czujniki i LCD
Bme280 bme280;
GroveMP503 groveMp503;
Lcd lcd;

// ⏱️ Zmienne czasowe
unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_INTERVAL = 500; // co 0.5 s próbka

void setup()
{
  Serial.begin(9600);
  delay(500);

  bme280.begin(SDA_PIN, SCL_PIN);
  groveMp503.begin(SIGNAL_PIN);
  lcd.begin();

  pinMode(2, OUTPUT); // LED status WiFi

  sender.begin(ssid, password);

  Serial.println("✅ Setup zakończony. Start pomiarów...");
}

void loop()
{
  unsigned long now = millis();

  // 🔹 Zbieranie próbek co 0.5 s
  if (now - lastSampleTime >= SAMPLE_INTERVAL)
  {
    lastSampleTime = now;

    Bme280Data envData = bme280.readValues();
    GroveMP503Data airData = groveMp503.readAirQuality();

    SensorSample sample;
    sample.temperature = envData.temperature;
    sample.humidity = envData.humidity;
    sample.pressure = envData.pressure;
    sample.voltage = airData.voltage;
    sample.airQuality = String(airData.quality.c_str());

    sender.addSample(sample); // dodanie próbki do bufora

    lcd.displayData(envData.temperature, envData.humidity, envData.pressure, sample.airQuality);

    Serial.printf("Temp: %.2f°C | Hum: %.2f%% | Press: %.2f hPa | Air: %s\n",
                  envData.temperature, envData.humidity, envData.pressure, sample.airQuality.c_str());
  }

  // 🔹 Wysyłka faktyczna co 5 s na oba endpointy
  sender.update(ssid, password);

  // 🔹 Dioda statusu WiFi
  digitalWrite(2, WiFi.status() == WL_CONNECTED ? HIGH : LOW);
}
