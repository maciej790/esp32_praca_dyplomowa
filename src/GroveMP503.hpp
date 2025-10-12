#include <Arduino.h>
using namespace std;

class GroveMP503{
    public:
        int signalPin; 
        int voltageValue = 0;
        string quality = "";

        void begin(int signal){
            this->signalPin = signal;
            pinMode(signalPin, INPUT);
        }

        double getVoltageFromSensor(){
            double sensorValue = analogRead(signalPin);
            voltageValue = (sensorValue * 3300) / 4095;
            return voltageValue; //mega volts!
        }

        void readAirQualityValue(){
            // Fresh	0 – 1000
            // Good	1001 – 1500
            // Moderate	1501 – 2000
            // Poor	2001 – 2500
            // Very Poor	2501 – 3300

            if (voltageValue <= 1000) quality = "Fresh";
            else if (voltageValue <= 1500) quality = "Good";
            else if (voltageValue <= 2000) quality = "Moderate";
            else if (voltageValue <= 2500) quality = "Poor";
            else quality = "Very Poor";
            
            Serial.print("Czystość powietrza: ");
            Serial.println(quality.c_str());
        }
};