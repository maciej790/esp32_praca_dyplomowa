#pragma once
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

class HttpSender
{
private:
    String serverUrl;

public:
    HttpSender(const String &url) : serverUrl(url) {}

    void begin(const char *ssid, const char *password)
    {
        Serial.print("Łączenie z WiFi...");
        WiFi.begin(ssid, password);

        int max_attempts = 20;
        int attempts = 0;

        while (WiFi.status() != WL_CONNECTED && attempts < max_attempts)
        {
            delay(500);
            Serial.print(".");
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("\nPołączono z WiFi!");
            Serial.print("Adres IP: ");
            Serial.println(WiFi.localIP());
        }
        else
        {
            Serial.println("\nNie udało się połączyć z WiFi.");
        }
    }

    bool sendData(float temperature, float humidity, float pressure, float voltage, const String &airQuality)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Brak połączenia z WiFi!");
            return false;
        }

        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{";
        jsonPayload += "\"temperature\": " + String(temperature, 2) + ",";
        jsonPayload += "\"humidity\": " + String(humidity, 2) + ",";
        jsonPayload += "\"pressure\": " + String(pressure, 2) + ",";
        jsonPayload += "\"voltage\": " + String(voltage, 2) + ",";
        jsonPayload += "\"airQuality\": \"" + airQuality + "\"";
        jsonPayload += "}";

        Serial.println("Wysyłam JSON:");
        Serial.println(jsonPayload);

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0)
        {
            Serial.print("Dane wysłane, HTTP code: ");
            Serial.println(httpResponseCode);
            Serial.println("Odpowiedź serwera: " + http.getString());
            http.end();
            return true;
        }
        else
        {
            Serial.print("Błąd wysyłania: ");
            Serial.println(httpResponseCode);
            http.end();
            return false;
        }
    }
};
