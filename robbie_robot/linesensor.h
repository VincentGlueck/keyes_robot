#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __LINESENSOR__

#define __LINESENSOR__

// Line sensors are cute but not very useful for autonom driving

#define L_pin 11  //left
#define M_pin 7   //middle
#define R_pin 8   //right

int L_val, M_val, R_val;
int L_old, M_old, R_old;

unsigned char line_white[] = { 0x60, 0x90, 0x90, 0x60 };
unsigned char line_black[] = { 0x60, 0xf0, 0xf0, 0x60 };

void setup_line_sensor() {
  pinMode(L_pin, INPUT);
  pinMode(M_pin, INPUT);
  pinMode(R_pin, INPUT);
}

void do_line_sensor() {
  L_val = digitalRead(L_pin);
  M_val = digitalRead(M_pin);
  R_val = digitalRead(R_pin);
}

void show_line_sensors() {
  matrix_display(line);
  if (L_val != L_old || M_val != M_old || R_val != R_old) {
    Serial.println("Line sensor change detected...");
    Serial.print(L_val);
    Serial.print("  ");
    Serial.print(M_val);
    Serial.print("  ");
    Serial.print(R_val);
    Serial.println("  ");
    L_old = L_val;
    M_old = M_val;
    R_old = R_val;
  }
}

#endif