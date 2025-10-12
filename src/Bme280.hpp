#pragma once
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

class Bme280 {
  private:
    Adafruit_BME280 bme;

  public:
    void begin(int sda, int scl) {
      // begin wire I2C
      Wire.begin(sda, scl);

      // check sensor connect
      if (!bme.begin(0x76, &Wire)) {
        Serial.println("Nie wykryto czujnika BME280!");
        Serial.print("SensorID: 0x");
        Serial.println(bme.sensorID(), 16);
        while (true) delay(10);
      }

      Serial.println("BME280 wykryty!");
    }

    void readValuesFromSensor() {
      Serial.print("Temperatura: ");
      Serial.print(bme.readTemperature());
      Serial.println(" °C");

      Serial.print("Ciśnienie: ");
      Serial.print(bme.readPressure() / 100.0F);
      Serial.println(" hPa");

      Serial.print("Wilgotność: ");
      Serial.print(bme.readHumidity());
      Serial.println(" %");

      delay(2000);
    }
};
