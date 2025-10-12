#include <Arduino.h>
#include "led.hpp"  

const int LEDPIN = 2;

Led led(LEDPIN);

void setup() {
    pinMode(LEDPIN, OUTPUT);
}

void loop() {
    led.blinkLed();
    delay(1000);
    led.offLed();
    delay(100);
}
