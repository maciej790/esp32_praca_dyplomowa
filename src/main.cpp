#include <Arduino.h>
#include <WiFi.h>

#include "Bme280.hpp"
#include "GroveMP503.hpp"
#include "Lcd.hpp"
#include "HttpSender.hpp"
#include "connection.h" // definiuje WIFI_SSID i WIFI_PASS

// 🧩 Piny I2C i czujników
const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int SIGNAL_PIN = 35;

// 🌍 Obiekt HttpSender
HttpSender sender("http://192.168.0.7:3000/sensor_data");

// 🧠 Czujniki i LCD
Bme280 bme280;
GroveMP503 groveMp503;
Lcd lcd;

// ⏱️ Zmienne czasowe
unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_INTERVAL = 500; // co 0,5 s próbka

unsigned long lastLcdUpdate = 0;
const unsigned long LCD_INTERVAL = 5000; // 5 s odświeżanie LCD

void setup()
{
  Serial.begin(9600);
  delay(500);

  // 🌐 WiFi w trybie STA
  Serial.println("Uruchamianie WiFi w trybie STA...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Łączenie z siecią WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nPołączono z WiFi!");
  Serial.print("Adres ESP32 (STA): ");
  Serial.println(WiFi.localIP());

  // 📟 Inicjalizacja czujników i LCD
  bme280.begin(SDA_PIN, SCL_PIN);
  groveMp503.begin(SIGNAL_PIN);
  lcd.begin();

  pinMode(2, OUTPUT); // LED status WiFi

  sender.begin(); // HttpSender gotowy

  Serial.println("✅ Setup zakończony. Start pomiarów...");
}

void loop()
{
  unsigned long now = millis();

  // Odczyt próbek z czujników co 0,5 s
  if (now - lastSampleTime >= SAMPLE_INTERVAL)
  {
    lastSampleTime = now;

    // Odczyt danych z BME280 i GroveMP503
    Bme280Data envData = bme280.readValues();
    GroveMP503Data airData = groveMp503.readAirQuality();

    // Przygotowanie próbki do wysyłki - tworzenie struktury SensorSample
    SensorSample sample;
    sample.temperature = envData.temperature;
    sample.humidity = envData.humidity;
    sample.pressure = envData.pressure;
    sample.voltage = airData.voltage;

    // Kopiowanie jakości powietrza do tablicy char
    strncpy(sample.airQuality, airData.quality.c_str(), sizeof(sample.airQuality) - 1);
    sample.airQuality[sizeof(sample.airQuality) - 1] = '\0';

    // Dodanie próbki do bufora wysyłkowego
    sender.addSample(sample);

    // Wyświetlanie na LCD co 5 sekund
    if (now - lastLcdUpdate >= LCD_INTERVAL)
    {
      lastLcdUpdate = now;
      lcd.displayData(
          envData.temperature,
          envData.humidity,
          envData.pressure,
          sample.airQuality);
    }

    // Debug na Serial Monitor
    Serial.printf(
        "Temp: %.2f°C | Hum: %.2f%% | Press: %.2f hPa | Air: %s\n",
        envData.temperature,
        envData.humidity,
        envData.pressure,
        sample.airQuality);
  }

  // 🔹 Wysyłka ostatniej próbki co 5 s
  sender.update();

  // 🔹 Dioda statusu WiFi
  digitalWrite(2, WiFi.status() == WL_CONNECTED ? HIGH : LOW);
}
