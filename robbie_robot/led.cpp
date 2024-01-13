#include "Arduino.h"
#ifndef __LED__
#include "led.h"
#endif

LED::LED() {
  pinMode(LED_PIN, LOW);
}

void LED::off() {
  digitalWrite(LED_PIN, LOW);
}

void LED::on() {
  digitalWrite(LED_PIN, HIGH);
}

void LED::setBlink(uint8_t value) {
  blink = value;
}