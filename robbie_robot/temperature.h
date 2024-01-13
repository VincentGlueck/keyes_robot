#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __TEMPERATURE__

#define __TEMPERATURE__

#include <DHT.h>

#define TEMPERATURE_PIN 9

float temperature;
float huminity;

DHT dht;

void setup_temperature() {
  dht.setup(TEMPERATURE_PIN);
}

void do_temp_sensor() {
  char buff[10];
  if((global_cnt & 0xFF) == 0xFF) {
    float t = dht.getTemperature();
    float h = dht.getHumidity();
    if("OK" == dht.getStatusString()) {
      temperature = t;
      huminity = h;
      Serial << "temp " << temperature <<", hum " << huminity << "\n";
      /*
      dtostrf(temperature, 4, 1, buff);
      scroll_msg = buff;
      scroll_msg += " C - ";
      dtostrf(huminity, 4, 1, buff);
      scroll_msg += buff;
      Serial.println(scroll_msg);
      matrix_what = MATRIX_SCROLL_TEXT;
      */
    }
  }
}

#endif