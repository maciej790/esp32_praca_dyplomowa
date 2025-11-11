#pragma once
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

struct SensorSample
{
    float temperature;
    float humidity;
    float pressure;
    float voltage;
    String airQuality;
};

class HttpSender
{
private:
    String serverUrl;
    SensorSample buffer[20]; // bufor do 20 próbek
    int bufferIndex = 0;
    const int maxRetries = 3;

public:
    HttpSender(const String &url) : serverUrl(url) {}

    void begin(const char *ssid, const char *password)
    {
        Serial.print("Łączenie z WiFi...");
        WiFi.begin(ssid, password);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20)
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

    // Sprawdza WiFi i ewentualnie ponownie łączy
    void ensureWiFiConnected(const char *ssid, const char *password)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi zerwane, ponowne łączenie...");
            WiFi.disconnect();
            WiFi.begin(ssid, password);

            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20)
            {
                delay(500);
                Serial.print(".");
                attempts++;
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("\nPołączono ponownie z WiFi!");
            }
            else
            {
                Serial.println("\nNie udało się ponownie połączyć z WiFi.");
            }
        }
    }

    // Dodaj próbkę do bufora i wyślij jeśli bufor pełny
    void addSample(SensorSample sample, const char *ssid, const char *password)
    {
        buffer[bufferIndex++] = sample;

        if (bufferIndex >= 20)
        {
            sendBufferedData(ssid, password);
            bufferIndex = 0;
        }
    }

    // Wyślij wszystkie próbki z bufora
    void sendBufferedData(const char *ssid, const char *password)
    {
        ensureWiFiConnected(ssid, password);

        for (int i = 0; i < bufferIndex; i++)
        {
            sendDataWithRetry(buffer[i]);
        }
    }

private:
    // Wysyłka HTTP z retry
    bool sendDataWithRetry(SensorSample sample)
    {
        for (int attempt = 0; attempt < maxRetries; attempt++)
        {
            if (sendData(sample))
                return true;
            Serial.println("Próba ponownego wysłania...");
            delay(1000);
        }
        Serial.println("Nie udało się wysłać próbki po kilku próbach.");
        return false;
    }

    // Wysyłka jednej próbki
    bool sendData(SensorSample sample)
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
        jsonPayload += "\"temperature\": " + String(sample.temperature, 2) + ",";
        jsonPayload += "\"humidity\": " + String(sample.humidity, 2) + ",";
        jsonPayload += "\"pressure\": " + String(sample.pressure, 2) + ",";
        jsonPayload += "\"voltage\": " + String(sample.voltage, 2) + ",";
        jsonPayload += "\"airQuality\": \"" + sample.airQuality + "\"";
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
