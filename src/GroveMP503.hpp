#pragma once
#include <Arduino.h>
#include <string>
using namespace std;

struct GroveMP503Data
{
  double voltage; // napięcie z czujnika w mV
  string quality; // opisowa jakość powietrza
};

class GroveMP503
{
private:
  int signalPin;
  double voltageValue = 0.0;

public:
  void begin(int signal)
  {
    signalPin = signal;
    pinMode(signalPin, INPUT);
  }

  double getVoltageFromSensor()
  {
    double sensorValue = analogRead(signalPin);
    voltageValue = (sensorValue * 3300.0) / 4095.0; // przelicz na mV
    return voltageValue;
  }

  GroveMP503Data readAirQuality()
  {
    GroveMP503Data data;
    data.voltage = getVoltageFromSensor();
    Serial.println(getVoltageFromSensor());

    if (data.voltage <= 1000)
      data.quality = "Bardzo dobre";
    else if (data.voltage <= 1500)
      data.quality = "Dobre";
    else if (data.voltage <= 2000)
      data.quality = "Srednie";
    else if (data.voltage <= 2500)
      data.quality = "Zle";
    else
      data.quality = "Bardzo złe";

    return data;
  }
};
