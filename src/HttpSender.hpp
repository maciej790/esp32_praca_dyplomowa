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
    char airQuality[32]; // zamiast String
};

class HttpSender
{
private:
    String serverUrl;
    SensorSample buffer[20];
    int head = 0;  // indeks najstarszej próbki
    int count = 0; // liczba próbek w buforze
    const int maxRetries = 3;

    unsigned long lastSendTime = 0;
    const unsigned long SEND_INTERVAL = 5000;

public:
    HttpSender(const String &url) : serverUrl(url) {}

    void begin()
    {
        Serial.println("HttpSender gotowy. WiFi powinno być już połączone.");
    }

    void addSample(const SensorSample &sample)
    {
        if (count < 20)
        {
            int idx = (head + count) % 20;
            buffer[idx] = sample;
            count++;
        }
        else
        {
            // nadpisujemy najstarszą próbkę
            buffer[head] = sample;
            head = (head + 1) % 20;
        }
    }

    void update()
    {
        unsigned long now = millis();
        if (now - lastSendTime < SEND_INTERVAL || count == 0)
            return;

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Brak połączenia WiFi, pomijam wysyłkę.");
            lastSendTime = now;
            return;
        }

        // wysyłamy ostatnią próbkę (Twoja logika)
        int idx = (head + count - 1) % 20;
        sendDataWithRetry(buffer[idx], serverUrl);
        sendDataWithRetry(buffer[idx], "http://192.168.0.7:3000/sensor_data/store_data");

        lastSendTime = now;
    }

private:
    void sendDataWithRetry(const SensorSample &sample, const String &url)
    {
        for (int attempt = 0; attempt < maxRetries; attempt++)
        {
            if (sendData(sample, url))
                return;

            Serial.println("Próba ponownego wysłania...");
            delay(1000);
        }
        Serial.println("Nie udało się wysłać próbki po kilku próbach.");
    }

    bool sendData(const SensorSample &sample, const String &url)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Brak połączenia z WiFi!");
            return false;
        }

        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");

        // Tworzymy JSON w prosty, bezpieczny sposób
        char payload[256];
        snprintf(payload, sizeof(payload),
                 "{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"voltage\":%.2f,\"airQuality\":\"%s\"}",
                 sample.temperature, sample.humidity, sample.pressure, sample.voltage, sample.airQuality);

        Serial.println("📤 Wysyłam JSON do " + url + ":");
        Serial.println(payload);

        int code = http.POST(payload);
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
