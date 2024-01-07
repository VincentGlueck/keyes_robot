# Keyes robot

Inspired by

https://wiki.keyestudio.com/KS0555_Keyestudio_Mini_Caterpillar_Tank_Robot_V3.0_(Popular_Edition)

Little program to control a Keyes robot in a different way.

_Setup is with intended for use with light sensors and ultrasonic, incl. servo._

# Features

* 8833 Motor driver (left, right motor)
* Servo attached to Ultrasonic sensor
* Photoresistent x 2
* LED (used for "near-crash" notification)
* 16x8 LED matrix
* IR receiver (incl. in Motor driver ext. board)

# Settings

**(!) Keep in mind, this robot might drive (!)**

So, during development (with USB connected), **always** set

**_#define MOTOR_ACTIVE false_**

In this case, the robot does not drive, but simply shows that it would drive in the console.

_#define DISTANCE_CONSOLE false_

_#define SHOW_DISTANCE_MASK 0x7F_

Set to true to check distance on console. Mask used for how often console will disturb. Yes, right most should be "1".

_#define START_BY_DISTANCE true_

Set to true if robot should wake up on near hand pointing to ultrasonic.

_#define START_BY_LIGHT false_

Set to true if robot should wake up on bright light on photo sensors.

**Should not be combined with** #define FOLLOW_LIGHT true

This is a joke, the "eyes" will follow the light spread out by your phone (e.g.)


**_Optional_**:

https://github.com/MattFryer/Board_Identify - **used in setup()**


