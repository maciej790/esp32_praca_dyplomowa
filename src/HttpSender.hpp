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
    unsigned long lastSendTime = 0;
    const unsigned long SEND_INTERVAL = 5000; // wysyłka co 5 sekund

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
                Serial.println("\nPołączono ponownie z WiFi!");
            else
                Serial.println("\nNie udało się ponownie połączyć z WiFi.");
        }
    }

    void addSample(SensorSample sample)
    {
        if (bufferIndex < 20)
        {
            buffer[bufferIndex++] = sample;
        }
        else
        {
            // circular buffer – nadpisujemy najstarszą próbkę
            for (int i = 1; i < 20; i++)
                buffer[i - 1] = buffer[i];
            buffer[19] = sample;
        }
    }

    // Wywołanie w loop() – wysyłka ostatniej próbki co SEND_INTERVAL
    void update(const char *ssid, const char *password)
    {
        unsigned long now = millis();
        if (now - lastSendTime >= SEND_INTERVAL && bufferIndex > 0)
        {
            // wysyłka do pierwszego endpointu
            sendDataWithRetry(buffer[bufferIndex - 1], ssid, password, serverUrl);

            // wysyłka do drugiego endpointu (drugi URL możesz zmienić)
            sendDataWithRetry(buffer[bufferIndex - 1], ssid, password, "http://192.168.0.7:3000/sensor_data/store_data");

            lastSendTime = now;
        }
    }

private:
    bool sendDataWithRetry(SensorSample sample, const char *ssid, const char *password, String url)
    {
        ensureWiFiConnected(ssid, password);

        for (int attempt = 0; attempt < maxRetries; attempt++)
        {
            if (sendData(sample, url))
                return true;
            Serial.println("Próba ponownego wysłania...");
            delay(1000);
        }
        Serial.println("Nie udało się wysłać próbki po kilku próbach.");
        return false;
    }

    bool sendData(SensorSample sample, String url)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Brak połączenia z WiFi!");
            return false;
        }

        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{";
        jsonPayload += "\"temperature\": " + String(sample.temperature, 2) + ",";
        jsonPayload += "\"humidity\": " + String(sample.humidity, 2) + ",";
        jsonPayload += "\"pressure\": " + String(sample.pressure, 2) + ",";
        jsonPayload += "\"voltage\": " + String(sample.voltage, 2) + ",";
        jsonPayload += "\"airQuality\": \"" + sample.airQuality + "\"";
        jsonPayload += "}";

        Serial.println("📤 Wysyłam JSON do " + url + ":");
        Serial.println(jsonPayload);

        int code = http.POST(jsonPayload);
        http.end();

        if (code > 0 && code < 400)
        {
            Serial.printf("✅ Wysłano OK do %s (HTTP %d)\n", url.c_str(), code);
            return true;
        }
        else
        {
            Serial.printf("❌ Błąd wysyłania do %s (HTTP %d)\n", url.c_str(), code);
            return false;
        }
    }
};
