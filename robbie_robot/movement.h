#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __MOVEMENT__

#define MOVEMENT

#define BACKWARD_STOP_AFTER 60


void wait_for_start() {
  if (!START_BY_DISTANCE) {
    // start by distance is disabled
    matrix_what = MATRIX_STOP;
    return;
  }
  if ((global_cnt >> 8) & 1 == 1) {
    matrix_what = MATRIX_HOUR_GLASS;
  } else {
    if ((global_cnt >> 4) & 1 == 1) {
      matrix_what = MATRIX_WAIT_LEFT;
    } else {
      matrix_what = MATRIX_WAIT_RIGHT;
    }
  }
}


void do_car_movement() {
  if (!car_start) {
    if ((global_cnt & 0x7F) == 0x7F) {
      Serial.println("Wait for start...");
    }
    wait_for_start();
    return;
  }
  if (car_mode == CAR_ORIENTATION) {
    do_orientation();
    return;
  }
  if(car_mode != CAR_BACKWARD && !car_forced_backward) {
    car_backward_for = 0;
  } else {
    car_backward_for++;
    // old_car_mode = car_mode;  // TODO: this might be a problem
  }
  if (car_mode != old_car_mode)
    switch (car_mode) {
      case CAR_FORWARD: car_forward(); break;
      case CAR_WAIT: break;
      case CAR_BACKWARD:
        servo_target = LOOK_AHEAD;
        car_backward();
        break;
      case CAR_STOP:
        servo_target = LOOK_AHEAD;
        car_stop();
        break;
      case CAR_LEFT:
        servo_target = LOOK_AHEAD - 50;
        car_left();
        break;
      case CAR_RIGHT:
        servo_target = LOOK_AHEAD + 50;
        car_right();
        break;
      case CAR_IDLE: break;
      default:
        Serial << "!!! undef mode " << car_mode << "\n";
        break;
    }
  old_car_mode = car_mode;
  if ((global_cnt & 0x07) == 0x07) {
    if (car_mode == CAR_FORWARD) {
      if (distance < 45 && speed > SLOW_FORWARD) {
        speed -= 10;
      } else if (distance > 70 && speed < MAX_AUTO_INC_SPEED) {
        speed += 5;
      }
    }
  }
  
  if ((global_cnt & 0x0f) == 0x0f) {
    Serial << "mode: " << car_mode << ", cnt: " << car_cnt << ", spd: " << speed  << ", forw: " << car_forward_for << ", backw: " << car_backward_for << "\n";
  }
  
  #ifdef MOTOR_ACTIVE
  if (speed != old_speed && car_mode == CAR_FORWARD) {
    analogWrite(MR_PWM, 220 - (speed + RIGHT_WHEEL_EXTRA));
    analogWrite(ML_PWM, 220 - (speed + LEFT_WHEEL_EXTRA));
    old_speed = speed;
  }
  #endif
  if ((car_mode == CAR_LEFT || car_mode == CAR_RIGHT) && car_forced_turn) {
    if (car_cnt > left_right_time) {
      car_forced_turn = false;
      speed = forward_speed;
      #ifdef MOTOR_ACTIVE
      analogWrite(MR_PWM, 220 - (speed + RIGHT_WHEEL_EXTRA));
      analogWrite(ML_PWM, 220 - (speed + LEFT_WHEEL_EXTRA));
      #endif
      set_car_mode(CAR_FORWARD);
      car_forward_for = 0;
    }
  }
  if (car_forced_backward) {
    if (car_cnt > left_right_time) {
      car_forward_for = 0x80;
      car_forced_backward = false;
      set_car_mode(CAR_STOP);
      car_stop();
      matrix_what = MATRIX_DIZZY;
      start_orientation();
    }
  }
  if (car_mode == CAR_FORWARD) {
    servo_target = servo_target + servo_delta;
    if (servo_target > (LOOK_AHEAD + SERVO_MAX_TRACK_ANGLE)) {
      servo_delta = -SERVO_TRACK_INCREMENT;
    } else if (servo_target < (LOOK_AHEAD - SERVO_MAX_TRACK_ANGLE)) {
      servo_delta = SERVO_TRACK_INCREMENT;
    }
  }
  if ((global_cnt & 0x3) == 0x03) {
    if (car_mode == CAR_FORWARD) {
      car_forward_for++;
    }
    car_cnt++;
  }
  if(car_backward_for > BACKWARD_STOP_AFTER) {
    car_stop();
    car_mode = CAR_STOP;
    car_forward_for = 30;
    start_orientation();
  }
}


#endif