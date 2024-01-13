#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __NEO_PIXEL__

#define __NEO_PIXEL__

#include <Adafruit_NeoPixel.h>

#define LED_6812_PIN A2
#define LED_COUNT 4
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
#define NEO_PIXEL_BRIGHTNESS 30

Adafruit_NeoPixel strip(LED_COUNT, LED_6812_PIN, NEO_GRB + NEO_KHZ800);
uint32_t colors[4] = { 0, 0, 0, 0 };

void set_neopixel_brightness(uint8_t value) {
  strip.setBrightness(value);
}

void setup_led_6812() {
  strip.begin();
  strip.show();
  set_neopixel_brightness(NEO_PIXEL_BRIGHTNESS);
}

void do_led_6812() {
  if ((global_cnt & 0x07) == 0x07) {
    for (int n = 0; n < strip.numPixels(); n++) {
      if (n < (strip.numPixels() - 1)) colors[n] = colors[n + 1];
      else colors[n] = strip.Color(rnd(), rnd(), rnd());
      strip.setPixelColor(n, colors[n]);
    }
    strip.show();
  }
}

#endif