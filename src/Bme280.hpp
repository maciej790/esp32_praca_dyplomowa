#pragma once
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

struct Bme280Data {
  float temperature;
  float pressure;
  float humidity;
  float altitude;
};

class Bme280 {
  private:
    Adafruit_BME280 bme;
    bool initialized = false;

  public:
    void begin(int sda, int scl) {
      Wire.begin(sda, scl);

      // Próba inicjalizacji z dwoma możliwymi adresami I2C
      bme.begin(0x76);
     // bme.begin(0x77);
    }

    Bme280Data readValues() {
      Bme280Data data;
      
      data.temperature = bme.readTemperature();
      data.pressure = bme.readPressure() / 100.0F;
      data.humidity = bme.readHumidity();
      data.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

      return data;
    }
};
