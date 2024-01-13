// LED.h
#ifndef __LED__
#define __LED__

#include <Arduino.h>


#include "global.h"

#define LED_PIN A1
uint8_t blink;

void setup_led() {
  pinMode(LED_PIN, LOW);
}

void led_on() {
  pinMode(LED_PIN, HIGH);
}

void led_off() {
  pinMode(LED_PIN, LOW);
}

void led_set(uint8_t value) {
  blink = value;
}

void do_led() {
  if(blink != 0xff && (global_cnt & 0x1f) == 0x1f) {
    blink = !blink;
    pinMode(LED_PIN, blink);
  }
}

#endif