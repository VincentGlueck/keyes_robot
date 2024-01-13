#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __IR__

#include "robot_ctrl.h"
#include "neopixel.h"
#include "ultrasonic.h"

#define __IR__
#include <IRremote.h> // http://github.com/shirriff/Arduino-IRremote 

// show IR codes received?
#define PRINT_IR_CODES false
#define PRINT_IR_MANUFACTURER false  // combine true with PRINT_IR_CODES true, ok?

// IR control
#define IR_PIN 3

// IR key codes, you may want to modify these
#define IR_1 0xFF6897
#define IR_2 0xFF9867
#define IR_3 0xFFB04F
#define IR_4 0xFF30CF
#define IR_5 0xFF18E7
#define IR_6 0xFF7A85
#define IR_7 0xFF10EF
#define IR_8 0xFF38C7
#define IR_9 0xFF5AA5
#define IR_0 0xFF4AB5
#define IR_STAR 0xFF42BD
#define IR_HASH 0xFF52AD
#define IR_UP 0xFF629D
#define IR_LEFT 0xFF22DD
#define IR_RIGHT 0xFFC23D
#define IR_DOWN 0xFFA857
#define IR_OK 0xFF02FD
#define IR_SKIP 0xFFFFFFFF

IRrecv irrecv(IR_PIN);
long ir_rec;
decode_results ir_results;

void setup_ir() {
  irrecv.enableIRIn();
}

void do_IR() {
  if (irrecv.decode(&ir_results)) {
    ir_rec = ir_results.value;
    if (PRINT_IR_CODES && PRINT_IR_MANUFACTURER) {
      String type = "UNKNOWN";
      String typelist[14] = { "UNKNOWN", "NEC", "SONY", "RC5", "RC6", "DISH", "SHARP", "PANASONIC", "JVC", "SANYO", "MITSUBISHI", "SAMSUNG", "LG", "WHYNTER" };
      if (ir_results.decode_type >= 1 && ir_results.decode_type <= 13) {
        type = typelist[ir_results.decode_type];
        Serial << "IR TYPE: " << type << ", ";
      }
    }
    if (PRINT_IR_CODES || PRINT_IR_MANUFACTURER) {
      if (ir_rec != IR_SKIP) {
        Serial.print("ir_rec: ");
        Serial.println(ir_rec, HEX);
      }
    }
    switch (ir_rec) {
      case IR_1: speed = SLOW_FORWARD; break;
      case IR_2: speed = SLOW_FORWARD + 15; break;
      case IR_3: speed = SLOW_FORWARD + 30; break;
      case IR_4: speed = SLOW_FORWARD + 50; break;
      case IR_5: speed = SLOW_FORWARD + 70; break;
      case IR_6: speed = SLOW_FORWARD + 95; break;
      case IR_7: speed = SLOW_FORWARD + 120; break;
      // own risk!
      case IR_8: speed = SLOW_FORWARD + 150; break;
      case IR_9: speed = SLOW_FORWARD + 180; break;
      case IR_0: car_mode = CAR_STOP; break;
      case IR_UP:
        {
          car_start = true;
          car_mode = CAR_FORWARD;
          set_neopixel_brightness(NEO_PIXEL_BRIGHTNESS);
          break;
        }
      case IR_LEFT: car_mode = CAR_LEFT; break;
      case IR_RIGHT: car_mode = CAR_RIGHT; break;
      case IR_DOWN:
        if (car_mode == CAR_FORWARD) car_mode = CAR_STOP;
        else {
          car_mode = CAR_BACKWARD;
          car_forced_backward = false;
        }
        break;
      case IR_OK: car_mode = CAR_STOP; break;
      case IR_HASH:
        set_neopixel_brightness(0);
        look(LOOK_AHEAD);
        car_mode = CAR_STOP;
        car_start = false;
        break;
    }
    irrecv.resume();
  }
}

#endif