#pragma once
#include <Arduino.h>

class Led
{
    public:
        int pin;

        Led(int pin)
        {
            this->pin = pin;
        }
    
        void blinkLed();
        void offLed();
};

void Led::blinkLed(){digitalWrite(pin, HIGH);}
void Led::offLed(){digitalWrite(pin, LOW);}