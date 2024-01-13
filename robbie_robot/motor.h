#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __MOTOR__

#define __MOTOR__

#define ML_Ctrl 4  // direction left motor
#define ML_PWM 6   // PWM control left motor
#define MR_Ctrl 2  // direction right motor
#define MR_PWM 5   // PWM right motor

void car_forward() {
  matrix_what = MATRIX_FORWARD;
  #ifdef MOTOR_ACTIVE
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, 220 - (speed + RIGHT_WHEEL_EXTRA));
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, 220 - (speed + LEFT_WHEEL_EXTRA));
  #endif
}

void car_backward() {
  if(matrix_what != MATRIX_DIZZY) matrix_what = MATRIX_BACKWARD;
  #ifdef MOTOR_ACTIVE
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 80);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 80);
  #endif
}

void car_left() {
  matrix_what = MATRIX_LEFT;
  car_cnt = 0;
  #ifdef MOTOR_ACTIVE
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 110);
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, 180);
  #endif
}

void car_right() {
  matrix_what = MATRIX_RIGHT;
  car_cnt = 0;
  #ifdef MOTOR_ACTIVE
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, 180);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 110);
  #endif
}
void car_stop() {
  matrix_what = MATRIX_STOP;
  #ifdef MOTOR_ACTIVE
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 0);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 0);
  #endif
}

#endif