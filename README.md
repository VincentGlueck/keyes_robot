# Keyes robot / "The Robbie Robot"

Inspired by

https://www.keyestudio.com/

https://wiki.keyestudio.com/KS0555_Keyestudio_Mini_Caterpillar_Tank_Robot_V3.0_(Popular_Edition)

Little program to control a Keyes robot in a different way.

# Features

* 8833 Motor driver (left, right motor)
* Servo attached to Ultrasonic sensor
* LED (used for "near-crash" notification)
* 16x8 LED NEO_pixels matrix 
* IR receiver
* DHT temperature sensor, replacing Photoresistent (1)
* 6812 4 LED, replacing Photoresistent (2)

# Settings

**(!) Keep in mind, this robot might drive (!)**

So, during development (with USB connected), **always** set

**// _#define MOTOR_ACTIVE**
Robot will not drive, to get it alive, uncomment

**#define DISTANCE_CONSOLE**
Show current distance measured in Serial

**_#define START_BY_DISTANCE true_**
Set to true if robot should wake up on near hand pointing to ultrasonic.

__Note__: You may wake up robbie by "^" key on IR.

Uses

https://github.com/feilipu/Arduino_FreeRTOS_Library

https://github.com/adafruit/Adafruit_NeoPixel

https://www.arduino.cc/reference/en/libraries/servo/

https://github.com/adafruit/DHT-sensor-library

_(and others, I guess)_

![robbie_robot_github](https://github.com/VincentGlueck/keyes_robot/assets/139572548/5fff11a8-74e2-4316-81ad-939523db94a2)
