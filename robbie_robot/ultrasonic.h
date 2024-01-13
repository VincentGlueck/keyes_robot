#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __ULTRASONIC__

#define __ULTRASONIC__

#include <Servo.h>  // https://www.arduino.cc/reference/en/libraries/servo/
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "robot_ctrl.h"
#include "motor.h"
#include "neopixel.h"
#ifndef LED_h
#include "led.h"
#endif

#define DISTANCE_CONSOLE
#define SHOW_DISTANCE_MASK 0x7F  // how often will it show up

#define TRIG_PIN 13
#define ECHO_PIN 12
#define LOOK_AHEAD 90             // should be 90, but you know...
#define SERVO_MAX_TRACK_ANGLE 30  // look left/right during forward driving
#define SERVO_TRACK_INCREMENT 2   // angle++ on each n-th loop (see do_car_movement)

#define SERVO_PIN 10  // servo's PIN
#define LOOK_SETUP_TAKES 3  // init loop count of ultrasonic
uint8_t look_setup = 0;

LED led;
Servo servo;
uint8_t servo_angle = LOOK_AHEAD;
uint8_t servo_target = LOOK_AHEAD;
int8_t servo_delta = SERVO_TRACK_INCREMENT;

int distance;
long duration, cm, inches;

uint16_t dist_left, dist_right, cnt_left, cnt_right;

void setup_ultrasonic() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
}

void look(uint8_t direction) {
  servo.write(direction);
}

void decide_direction() {
  matrix_what = MATRIX_HOUR_GLASS;
  if (cnt_left > 0 && cnt_right > 0) {
    uint16_t av_left = dist_left / cnt_left;
    uint16_t av_right = dist_right / cnt_right;
    left_right_time = ((rnd() & 0x7) + 5) << 0;
    car_cnt = 0;
    if (av_left < 25 && av_right < 25) {
      left_right_time += 35;
      car_forced_backward = true;
      set_car_mode(CAR_BACKWARD);
      return;
    }
    car_forced_turn = true;
    speed = SLOW_FORWARD;
    forward_speed = speed;
    if (av_left > av_right) {
      set_car_mode(CAR_RIGHT);
    } else {
      set_car_mode(CAR_LEFT);
    }
  } else {
    matrix_what = MATRIX_DIZZY;
    left_right_time += 35;
    car_forced_backward = true;
    set_car_mode(CAR_BACKWARD);
  }
}

void start_orientation() {
  if (car_forward_for < 10) {
    led.setBlink(0);
    set_car_mode(CAR_BACKWARD);
    car_forced_backward = true;
    car_cnt = 0;
    left_right_time = ((rnd() & 0x7) + 3) << 0;
    return;
  }
  led.setBlink(0xff);
  set_car_mode(CAR_ORIENTATION);
  orientation_cnt = 0;
  dist_left = dist_right = cnt_left = cnt_right = 0;
}

void process_look_setup() {
  matrix_what = MATRIX_DIZZY;
  if ((global_cnt & 0x0f) == 0x0f) {
    switch (look_setup) {
      case 0: look(LOOK_AHEAD + 30); break;
      case 1: look(LOOK_AHEAD - 30); break;  // look left
      case 2:
        look(LOOK_AHEAD);
        Serial.println("Robbie Robot is ready...");
        break;
      default: break;
    }
    look_setup++;
  }
}

void do_distance() {
  if (look_setup < LOOK_SETUP_TAKES) {
    process_look_setup();
    return;
  }
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  //Give at least 10us high level trigger
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;  // we use cm, inch is similar
  boolean near_by = distance >= 2 && distance <= 20;
  if (near_by) {
    #ifdef START_BY_DISTANCE
    if (!car_start && distance <= 10) {
      car_cnt = 0;
      car_start = true;
      crash_ignore_cnt = CRASH_IGNORE_TIME;
      set_car_mode(CAR_FORWARD);
    }
    #endif
    if (crash_ignore_cnt == 0) {
      led.on();
      if (car_mode != CAR_ORIENTATION) {
        if (!car_forced_turn && !car_forced_backward) {
          set_car_mode(CAR_STOP);
          car_stop();
          matrix_what = MATRIX_STOP;
          start_orientation();
        }
      }
    }
  } else {
    led.off();
  }
  if (crash_ignore_cnt > 0 && car_start) {
    crash_ignore_cnt--;
  }

  #ifdef DISTANCE_CONSOLE
  if ((global_cnt & SHOW_DISTANCE_MASK) == SHOW_DISTANCE_MASK) {
    Serial << "dist: " << distance << " cm" << (near_by ? "  --> crashing\n" : "\n");
  }
  #endif
}

void do_orientation() {
  if ((global_cnt & 0x1F) == 0x1F) {
    if (orientation_cnt > 2) {
      orientation_cnt = 0;
      decide_direction();
    } else {
      switch (orientation_cnt) {
        case 0:
          look(LOOK_AHEAD - 70);
          matrix_what = MATRIX_LEFT;
          break;
        case 1:
          look(LOOK_AHEAD + 70);
          matrix_what = MATRIX_RIGHT;
          break;
        case 2:
          look(LOOK_AHEAD);
          matrix_what = MATRIX_STOP;
          break;
        default: break;
      }
      orientation_cnt++;
    }
  } else {
    if (orientation_cnt == 2) {
      dist_left += distance;
      cnt_left++;
    } else if (orientation_cnt == 1) {
      dist_right += distance;
      cnt_right++;
    }
  }
}

void do_servo() {
  if (car_mode == CAR_ORIENTATION) {
    return;
  }
  if (servo_angle > servo_target) {
    servo_angle -= 2;
    servo.write(servo_angle);
  } else if (servo_angle < servo_target) {
    servo_angle += 2;
    servo.write(servo_angle);
  }
}

#endif