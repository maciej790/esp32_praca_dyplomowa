#pragma once
#include <TFT_eSPI.h>
#include <SPI.h>

class Lcd
{
private:
  TFT_eSPI tft = TFT_eSPI(); // Obiekt TFT

public:
  void begin()
  {
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(2);
  }

  void displayData(float temp, float hum, float press, String quality)
  {
    tft.fillScreen(TFT_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.println("🌡️  Dane srodowiskowe");

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 50);
    tft.printf("Temp: %.1f C", temp);

    tft.setCursor(10, 80);
    tft.printf("Wilg: %.1f %%", hum);

    tft.setCursor(10, 110);
    tft.printf("Cisn: %.1f hPa", press);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 150);
    tft.printf("Powietrze: %s", quality.c_str());

    Serial.begin(9600);
    Serial.println("Dane wyswietlone na LCD.");
  }
};
