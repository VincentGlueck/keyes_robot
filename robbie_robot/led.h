// LED.h
#ifndef __LED__
#define __LED__

#include <Arduino.h>

#define LED_PIN A1

class LED {
  private:
    uint8_t blink;

  public:
    LED();
    void setBlink(uint8_t value);
    void on();
    void off();
};

#endif