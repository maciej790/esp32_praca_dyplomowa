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

Bme280 bme280;
GroveMP503 groveMp503;
Lcd lcd;
HttpSender sender("http://ptsv3.com/stacjaPogodowa");

void setup()
{
  Serial.begin(9600);
  delay(500);

  bme280.begin(SDA_PIN, SCL_PIN);
  groveMp503.begin(SIGNAL_PIN);
  lcd.begin();
  sender.begin(ssid, password);
}

void loop()
{
  Bme280Data envData = bme280.readValues();
  GroveMP503Data airData = groveMp503.readAirQuality();

  Serial.print("Temperatura: ");
  Serial.println(envData.temperature);
  Serial.print("Ciśnienie: ");
  Serial.println(envData.pressure);
  Serial.print("Wilgotność: ");
  Serial.println(envData.humidity);
  Serial.print("Jakość powietrza: ");
  Serial.println(airData.quality.c_str());
  Serial.println("------------------------");

  lcd.displayData(envData.temperature, envData.humidity, envData.pressure, String(airData.quality.c_str()));

  if (!sender.sendData(envData.temperature, envData.humidity, envData.pressure, String(airData.quality.c_str())))
  {
    Serial.println("Błąd wysyłania danych do serwera!");
  }

  delay(3000);
}
