#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __LED__

#define __LED__

#define LED_PIN A1

uint8_t led_blink = 0xff; // 0xff -> do nothing, 0/1 -> blink

void setup_led() {
  pinMode(LED_PIN, OUTPUT);
}

void do_led() {
  if(led_blink == 0xff) {
    digitalWrite(LED_PIN, 0);
  } else if (((global_cnt & 0x07) == 0x07)) {
    led_blink = led_blink ^ 1;
    digitalWrite(LED_PIN, led_blink);
  }
}

#endif