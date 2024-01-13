#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __ROBOT_CTRL__

#define __ROBOT_CTRL__

#define START_BY_DISTANCE true  // your hand -> ultrasonic, engines start

#define SLOW_FORWARD 30         // @6.8V (2x18650) should result in slow forward
#define MAX_AUTO_INC_SPEED 130  // do not use risky values, e.g. 200

#define CAR_WAIT 0
#define CAR_STOP 1
#define CAR_FORWARD 2
#define CAR_BACKWARD 3
#define CAR_LEFT 4
#define CAR_RIGHT 5
#define CAR_ORIENTATION 6
#define CAR_IDLE 7

#define CRASH_IGNORE_TIME 0x40  // delayed start of crash detection (if you start by hand -> ultrasonic)

#define RIGHT_WHEEL_EXTRA 8 // adjust to get robbie drive straigt forward
#define LEFT_WHEEL_EXTRA 0  // ^^^ or this

// vehicle's initial speed (quite slow)
uint8_t speed = SLOW_FORWARD;
uint8_t old_speed = speed;
uint8_t forward_speed = speed;
uint8_t car_mode = CAR_WAIT;
uint8_t old_car_mode = car_mode;

bool car_forced_turn = false;
bool car_forced_backward = false;
bool car_start = false;

uint16_t car_cnt = 0;
uint16_t left_right_time;
int orientation_cnt = -1;
int crash_ignore_cnt;

uint16_t car_forward_for = 0;  // counts how far forward was possible
uint16_t car_backward_for = 0;

void set_car_mode(uint8_t mode) {
  car_mode = mode;
}



#endif