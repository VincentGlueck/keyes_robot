/****************************************************************
 * DO NOT comment out UNTIL YOU DISCONNECTED ROBOT FROM USB !!! *
****************************************************************/

// ##################  THIS ONE: ##################

// #define MOTOR_ACTIVE

// ######### COMMENT OUT FOR TESTING!!! ###########

/*======================================================================================
 | Demo uses NO light sensors, NOR smoke sensors, or vent - it includes:               |
 | 4 LED 6812 multi color attached to A2 (see neopixel.h)                              |
 | DHT compatible temperature/huminity sensor attached to A9 (see temperature.h)       |
 |                                                                                     |
 | also attached:                                                                      |
 |    16x8 NeoPixel LED                                                                |
 |    single LED                                                                       |
 |    Motor extension board                                                            |
 |    Line sensor                                                                      |
 |    Ultrasonic sensor, mounted on Servo for path finding                             |
 ======================================================================================*/

#define LED_6812_MOUNTED
#define TEMP_SENSOR_MOUNTED
//#define LIGHT_SENSORS_MOUNTED // deprecated as they are not very useful

#include <Arduino_FreeRTOS.h>

#include "matrix.h"
#ifdef TEMP_SENSOR_MOUNTED
  #include "temperature.h"
#endif  
#ifdef LED_6812_MOUNTED
  #include "neopixel.h"
  
#endif
#include "linesensor.h"
#include "ir.h"
#include "robot_ctrl.h"
#include "ultrasonic.h"
#include "motor.h"
#include "movement.h"
#ifndef __LED___
#include "led.h"
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);


  #ifdef __TEMPERATURE__
    setup_temperature();
  #endif

  #ifdef __MATRIX__
    setup_matrix();
  #endif

  #ifdef __LINESENSOR__
    setup_line_sensor();
  #endif
    
  #ifdef __NEO_PIXEL__
    setup_led_6812();
  #endif

  #ifdef __ULTRASONIC__
    setup_ultrasonic();
  #endif

  #ifdef __IR__
    setup_ir();
  #endif
}

// this will be removed in a future version as loop() is not required when using tasks
void loop() {
  do_matrix();
#ifdef TEMP_SENSOR_MOUNTED
  do_temp_sensor();
#endif
  do_distance();
  do_line_sensor();
  do_car_movement();
#ifdef LED_6812_MOUNTED
  do_led_6812();
#endif
  // led.doLED();
  do_servo();
  do_IR();
  global_cnt++; // bad practice
  delay(10); // fixed delay == evil
}
