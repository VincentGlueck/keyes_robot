#include <IRremote.h>
#include <Servo.h>

/****************************************************************
 * DO NOT SET TO true UNTIL YOU DISCONNECTED ROBOT FROM USB !!! *
****************************************************************/

// ##################  THIS ONE: ##################

#define MOTOR_ACTIVE true

#define SLOW_FORWARD 30         // @6.8V this should result in slow forward
#define MAX_AUTO_INC_SPEED 130  // do not use risky values, e.g. 200

// ##### MIGHT DAMAGE YOUR TOY IF SET TO TRUE #####

// show distance on console?
#define DISTANCE_CONSOLE true
#define SHOW_DISTANCE_MASK 0x7F  // how often will it show up
// show IR codes received?
#define PRINT_IR_CODES false
#define PRINT_IR_MANUFACTURER false  // combine true with PRINT_IR_CODES true, ok?
// should engines start by holding hand near distance sensor?
#define START_BY_DISTANCE true
// should engines start by light (by phone) directly spotted at light sensors?
#define START_BY_LIGHT true
// "eyes" follow light, should not be combined with START_BY_LIGHT true
#define FOLLOW_LIGHT false

#define BACKWARD_STOP_AFTER 60

#define RIGHT_WHEEL_EXTRA 8
#define LEFT_WHEEL_EXTRA 0

// LED
#define LED_PIN 9
uint8_t led_blink = 0xff;

// ultrasonic
#define TRIG_PIN 13
#define ECHO_PIN 12
#define LOOK_AHEAD 90             // should be 90, but you know...
#define SERVO_MAX_TRACK_ANGLE 30  // look left/right during forward driving
#define SERVO_TRACK_INCREMENT 2   // angle++ on each n-th loop (see do_car_movement)

// matrix display
#define SCL_Pin A5            //set a pin of clock to A5
#define SDA_Pin A4            //set a data pin to A4
#define SWITCH_DISPLAY_SHR 7  // global_cnt >> this value -> switch display mode

int matrix_what = 4;  // set to display something else (4 is blank)

// line sensor
#define L_pin 11  //left
#define M_pin 7   //middle
#define R_pin 8   //right

// light sensors
#define light_L_Pin A2     // left
#define light_R_Pin A1     // right
#define LIGHT_TRIGGER 600  // trigger value for "this is bright, e.g. your phone's lamp will do"

// servo magic
#define SERVO_PIN 10  // servo's PIN
Servo servo;
uint8_t servo_angle = LOOK_AHEAD;
uint8_t servo_target = LOOK_AHEAD;
int8_t servo_delta = SERVO_TRACK_INCREMENT;

// motor control
#define ML_Ctrl 4  // direction left motor
#define ML_PWM 6   // PWM control left motor
#define MR_Ctrl 2  // direction right motor
#define MR_PWM 5   // PWM right motor

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

// matrix images
unsigned char arrow_left[] = { 0x00, 0x00, 0x00, 0x18, 0x18, 0x3c, 0x66, 0x00, 0x18, 0x18, 0x7c, 0x66, 0x00, 0x00, 0x00, 0x00 };
unsigned char arrow_right[] = { 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0x18, 0x18, 0x00, 0x66, 0x3c, 0x18, 0x18, 0x00, 0x00, 0x00 };
unsigned char arrow_forward[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x12, 0x09, 0x12, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char arrow_backward[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x48, 0x90, 0x48, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char skull[] = { 0x00, 0x00, 0x00, 0x00, 0x3c, 0x72, 0xd7, 0xdf, 0xdf, 0xd7, 0x72, 0x3c, 0x00, 0x00, 0x00, 0x00 };
unsigned char stop[] = { 0x2E, 0x2A, 0x3A, 0x00, 0x02, 0x3E, 0x02, 0x00, 0x3E, 0x22, 0x3E, 0x00, 0x3E, 0x0A, 0x00, 0x00 };
unsigned char line[] = { 0x00, 0x0f, 0x08, 0x00, 0x0f, 0x00, 0x0f, 0x02, 0x04, 0x0f, 0x00, 0x0f, 0x0b, 0x09, 0x00, 0x00 };
unsigned char wait_left[] = { 0x18, 0x24, 0x24, 0x18, 0x00, 0x70, 0x30, 0x50, 0x80, 0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x18 };
unsigned char wait_right[] = { 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x80, 0x40, 0x28, 0x18, 0x38, 0x00, 0x18, 0x24, 0x24, 0x18 };
unsigned char hour_glass[] = { 0x00, 0x00, 0x00, 0x00, 0x81, 0xc3, 0xe7, 0xff, 0xe7, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define MATRIX_FORWARD 0
#define MATRIX_LEFT 1
#define MATRIX_RIGHT 2
#define MATRIX_STOP 3
#define MATRIX_CLEAR 4
#define MATRIX_DIZZY 5
#define MATRIX_LINE_SENSORS 6
#define MATRIX_WAIT_LEFT 7
#define MATRIX_WAIT_RIGHT 8
#define MATRIX_HOUR_GLASS 9
#define MATRIX_SCROLL_RND 10
#define MATRIX_SCROLL_ARRAY 11
#define MATRIX_SCROLL_TEXT 12
#define MATRIX_BACKWARD 13

// unsigned char a_letter[] = { 0xe0, 0x78, 0x2e, 0x21, 0x2e, 0x78, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char slash[] = { 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x03, 0x00 };

static const char* CHARZ = " ABCDEFGHIJKLMNOPQRSTUVWXYZ012345679+-*/_-:!?°\0";

unsigned char PIXEL[] = { 0xfc, 0x26, 0x22, 0x26, 0xfc, 0x00, 0xfe, 0x92, 0x92, 0x6c, 0x00, 0x38, 0x44, 0x82, 0x82, 0x00,
                          0xfe, 0x82, 0x82, 0x44, 0x38, 0x00, 0xfe, 0x92, 0x92, 0x92, 0x00, 0xfe, 0x12, 0x12, 0x02, 0x00,
                          0x7c, 0x82, 0xa2, 0xa2, 0x64, 0x00, 0xfe, 0x10, 0x10, 0xfe, 0x00, 0x82, 0xfe, 0x82, 0x00,
                          0x42, 0x82, 0x82, 0x82, 0x7e, 0x00, 0xfe, 0x30, 0x28, 0xc6, 0x00, 0xfe, 0x80, 0x80, 0x80, 0x00,
                          0xfe, 0x04, 0x08, 0x04, 0xfe, 0x00, 0xfe, 0x0c, 0x10, 0x60, 0xfe, 0x00, 0x7c, 0x82, 0x82, 0x7c, 0x00,
                          0xfe, 0x22, 0x22, 0x1c, 0x00, 0x7c, 0x82, 0x82, 0x7c, 0x80, 0x00, 0xfe, 0x32, 0x52, 0x8c, 0x00,
                          0x8c, 0x92, 0x92, 0x62, 0x00, 0x02, 0x02, 0xfe, 0x02, 0x02, 0x00, 0x7e, 0x80, 0x80, 0xfe, 0x00,
                          0x06, 0x78, 0x80, 0x78, 0x06, 0x00, 0x7e, 0x80, 0x70, 0x80, 0x7e, 0x00, 0x82, 0x6c, 0x38, 0x6c, 0x82, 0x00,
                          0x06, 0x1c, 0xf0, 0x1c, 0x06, 0x00, 0xc2, 0xb2, 0x8a, 0x86, 0x00, 0x7c, 0xa2, 0x92, 0x8a, 0x7c, 0x00,
                          0x08, 0x04, 0xfe, 0x00, 0xc4, 0xa2, 0x92, 0x9a, 0x8c, 0x00, 0x82, 0x92, 0x92, 0xfe, 0x00,
                          0x18, 0x14, 0x12, 0xfe, 0x10, 0x00, 0x9e, 0x92, 0x92, 0x62, 0x00, 0x6e, 0x92, 0x92, 0x92, 0x60, 0x00,
                          0x02, 0xc2, 0x32, 0x0e, 0x00, 0x6c, 0x92, 0x92, 0x6c, 0x00, 0x4c, 0x92, 0x92, 0x7c, 0x00,
                          0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x48, 0x30, 0x1c, 0x30, 0x48, 0x00,
                          0xc0, 0x60, 0x38, 0x0c, 0x06, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x10, 0x10, 0x10, 0x00,
                          0xd8, 0xd8, 0x00, 0xde, 0xde, 0x00, 0x12, 0x2a, 0xaa, 0x24, 0x00, 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00 };

unsigned char scroll_img[16] = { 0 };

// matrix for line sensors
unsigned char line_white[] = { 0x60, 0x90, 0x90, 0x60 };
unsigned char line_black[] = { 0x60, 0xf0, 0xf0, 0x60 };

uint8_t za, zb, zc, zx, counter;  // fast random generator, see rnd()
uint16_t global_cnt = 0;          // global timing, simply increments in each loop

uint16_t old_matrix = 0xffff;
uint16_t scroll_pos;
uint16_t scrolled_for;
uint16_t scroll_feed_idx = 0;
boolean scroll_feed_repeat = false;
String scroll_msg = "ROBBIE ROBOT SAYS HELLO TO YOU";
unsigned char scroll_pixels[8] = { 0 };
int scroll_msg_pix_ptr = -1;
#define SCROLL_FEED_SLOWDOWN 0x1

uint16_t car_forward_for = 0;  // counts how far forward was possible
uint16_t car_backward_for = 0;

#define LOOK_SETUP_TAKES 3  // init loop count of ultrasonic
uint8_t look_setup = 0;

// line sensors
int L_val, M_val, R_val;
int L_old, M_old, R_old;

// light sensors
int left_light, right_light, last_light_event;

#define CAR_WAIT 0
#define CAR_STOP 1
#define CAR_FORWARD 2
#define CAR_BACKWARD 3
#define CAR_LEFT 4
#define CAR_RIGHT 5
#define CAR_ORIENTATION 6
#define CAR_IDLE 7

#define CRASH_IGNORE_TIME 0x30  // delayed start of crash detection (if you start by hand -> ultrasonic)

// vehicle's initial speed (quite slow)
uint8_t speed = SLOW_FORWARD;
uint8_t old_speed = speed;
uint8_t forward_speed = speed;
boolean car_forced_turn = false;
boolean car_forced_backward = false;
boolean car_start = false;
uint8_t car_mode = CAR_WAIT;
uint8_t old_car_mode = car_mode;
uint8_t last_mode_change = 0;
uint8_t mode_wanted = 0xff;
uint8_t mode_wanted_cnt;
uint16_t car_cnt = 0;
uint16_t left_right_time;
int orientation_cnt = -1;
int crash_ignore_cnt;

// orientation
uint16_t dist_left, dist_right, cnt_left, cnt_right;

template<typename T>
Print& operator<<(Print& printer, T value) {
  printer.print(value);
  return printer;
}

uint8_t rnd() {
  zx++;
  za = (za ^ zc ^ zx);
  zb = (zb + za);
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}

int distance;
long duration, cm, inches;

uint8_t last_msg;

void setup() {
  Serial.begin(9600);  // enough for serial console
  while (!Serial) true;

  // many PINs to init
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);

  pinMode(L_pin, INPUT);
  pinMode(M_pin, INPUT);
  pinMode(R_pin, INPUT);

  pinMode(light_L_Pin, INPUT);
  pinMode(light_R_Pin, INPUT);

  servo.attach(SERVO_PIN);

  irrecv.enableIRIn();

  // init fast random generator
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);
}

void IIC_start() {
  // used for matrix
  digitalWrite(SDA_Pin, HIGH);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, LOW);
}

void IIC_end() {
  // used for matrix
  digitalWrite(SCL_Pin, LOW);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, HIGH);
  delayMicroseconds(3);
}

void IIC_send(unsigned char send_data) {
  for (byte mask = 0x01; mask != 0; mask <<= 1) {
    if (send_data & mask) {
      digitalWrite(SDA_Pin, HIGH);
    } else {
      digitalWrite(SDA_Pin, LOW);
    }
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, LOW);
  }
}

void set_car_mode(uint8_t mode) {
  if(last_mode_change > 0) {
    Serial << "NO mode change, cnt: " << last_mode_change << "\n";
    mode_wanted = mode;
    mode_wanted_cnt = last_mode_change;
    return;
  }
  car_mode = mode;
  last_mode_change = 8;
}

void matrix_display(unsigned char matrix_value[]) {
  IIC_start();
  IIC_send(0xc0);
  for (int i = 0; i < 16; i++) {
    IIC_send(matrix_value[i]);
  }
  IIC_end();
  IIC_start();
  IIC_send(0x8A);
  IIC_end();
}

void clear_scroll_img() {
  for (int i = 0; i < 16; i++) {
    scroll_img[i] = 0;
  }
  scrolled_for = 0;
}

void show_dizzy() {
  unsigned char show[16] = { 0 };
  for (int i = 0; i < 16; i++) {
    show[i] = rnd();
  }
  matrix_display(show);
}

void clear_matrix() {
  unsigned char show[16] = { 0 };
  matrix_display(show);
}

void show_scroll() {
  for (int i = 0; i < 15; i++) {
    scroll_img[i] = scroll_img[i + 1];
  }
  scrolled_for++;
  if (scrolled_for < ((1 << SWITCH_DISPLAY_SHR) - 16)) {
    scroll_img[15] = rnd();
  } else {
    scroll_img[15] = 0;
  }
  matrix_display(scroll_img);
}

void show_scroll_feed(unsigned char* feed) {
  for (int i = 0; i < 15; i++) {
    scroll_img[i] = scroll_img[i + 1];
  }
  if (scroll_feed_idx == 0xFFFF) {
    scroll_img[15] = 0;
  } else {
    unsigned char c = feed[scroll_feed_idx++];
    if (c == 0x00) {
      if (scroll_feed_repeat) scroll_feed_idx = 0;
      else scroll_feed_idx = 0xFFFF;
      scroll_img[15] = 0;
    } else {
      scroll_img[15] = c == 0xff ? 0 : c;
    }
  }
  matrix_display(scroll_img);
}

int get_pixel_offset(int num) {
  int pix_ptr = 0;
  while (num > 0) {
    pix_ptr++;
    if (PIXEL[pix_ptr] == 0x00) {
      if (PIXEL[pix_ptr + 1] == 0x00) {
        num = -127;
        break;
      }
      num--;
    }
  }
  return num == -127 ? -1 : pix_ptr;
}

void show_scroll_msg() {
  if (scroll_msg_pix_ptr < 0) {
    if (scroll_pos < scroll_msg.length()) {
      scroll_msg_pix_ptr = 0;
      int num = -1;
      char c = scroll_msg.charAt(scroll_pos);
      for (int i = 0; CHARZ[i] != 0x0; i++) {
        if (CHARZ[i] == c) {
          num = i;
          break;
        }
      }
      if (num == -1) num = 0;
      if (num == 0) {  // ' ' is special
        for (int n = 0; n < 6; n++) scroll_pixels[n] = 0x00;
      } else {
        int pix_ptr = get_pixel_offset(num - 1);
        if (pix_ptr >= 0) {
          int j = 0;
          while (PIXEL[++pix_ptr] != 0x00 && j < 6) {
            scroll_pixels[j++] = PIXEL[pix_ptr];
          }
          while (j < 6) scroll_pixels[j++] = 0x0;
        }
      }
    }
  }
  for (int i = 0; i < 15; i++) {
    scroll_img[i] = scroll_img[i + 1];
  }
  if (scroll_pos >= scroll_msg.length()) {
    scroll_img[15] = 0x00;
  } else if (scroll_msg_pix_ptr > 5) {
    scroll_msg_pix_ptr = -1;
    scroll_img[15] = 0x00;
    scroll_pos++;
  } else {
    scroll_img[15] = scroll_pixels[scroll_msg_pix_ptr++];
  }
  matrix_display(scroll_img);
}

void do_matrix() {
  switch (matrix_what) {
    case MATRIX_FORWARD: matrix_display(arrow_forward); break;
    case MATRIX_LEFT: matrix_display(arrow_left); break;
    case MATRIX_RIGHT: matrix_display(arrow_right); break;
    case MATRIX_STOP: matrix_display(stop); break;
    case MATRIX_CLEAR: clear_matrix(); break;
    case MATRIX_DIZZY: show_dizzy(); break;
    case MATRIX_LINE_SENSORS: show_line_sensors(); break;
    case MATRIX_WAIT_LEFT: matrix_display(wait_left); break;
    case MATRIX_WAIT_RIGHT: matrix_display(wait_right); break;
    case MATRIX_HOUR_GLASS: matrix_display(hour_glass); break;
    case MATRIX_SCROLL_RND:
      {
        if (old_matrix != matrix_what) {
          clear_scroll_img();
        }
        show_scroll();
        break;
      }
    case MATRIX_SCROLL_ARRAY:
      {
        if (old_matrix != matrix_what) {
          scroll_feed_idx = 0;
        }
        if ((global_cnt & SCROLL_FEED_SLOWDOWN) == SCROLL_FEED_SLOWDOWN) {
          show_scroll_feed(slash);
        }
        break;
      }
    case MATRIX_SCROLL_TEXT:
      {
        if (old_matrix != matrix_what) {
          if (scroll_msg.length() == 0) scroll_msg = "?text?";
          scroll_pos = 0;
          scroll_msg_pix_ptr = -1;
          scroll_msg.toUpperCase();
          clear_scroll_img();
        }
        if ((global_cnt & SCROLL_FEED_SLOWDOWN) == SCROLL_FEED_SLOWDOWN) {
          show_scroll_msg();
        }
        break;
      }
    case MATRIX_BACKWARD: matrix_display(arrow_backward); break;
    default: show_dizzy(); break;
  }
  old_matrix = matrix_what;
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

void do_line_sensor() {
  L_val = digitalRead(L_pin);  //Read the value of the left sensor
  M_val = digitalRead(M_pin);  //Read the value of the middle sensor
  R_val = digitalRead(R_pin);  //Read the value of the right sensor
}

void start_orientation() {
  if (car_forward_for < 10) {
    led_blink = 0;
    set_car_mode(CAR_BACKWARD);
    car_forced_backward = true;
    car_cnt = 0;
    left_right_time = ((rnd() & 0x7) + 3) << 0;
    return;
  }
  led_blink = 0xff;
  set_car_mode(CAR_ORIENTATION);
  orientation_cnt = 0;
  dist_left = dist_right = cnt_left = cnt_right = 0;
}

void do_distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  //Give at least 10us high level trigger
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;  // we use cm, inch is similar
  boolean near_by = distance >= 2 && distance <= 20;
  if (near_by) {
    if (START_BY_DISTANCE && !car_start && distance <= 10) {
      car_cnt = 0;
      car_start = true;
      crash_ignore_cnt = CRASH_IGNORE_TIME;
      Serial.println("Crash ignore");
      set_car_mode(CAR_FORWARD);
    }
    if (crash_ignore_cnt == 0) {
      digitalWrite(LED_PIN, HIGH);
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
    digitalWrite(LED_PIN, LOW);
  }
  if (crash_ignore_cnt > 0 && car_start) {
    crash_ignore_cnt--;
  }

  if (DISTANCE_CONSOLE && (global_cnt & SHOW_DISTANCE_MASK) == SHOW_DISTANCE_MASK) {
    Serial << "dist: " << distance << " cm" << (near_by ? "  --> crashing\n" : "\n");
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
  // Serial << "servo: " << servo_angle << ", target: " << servo_target << "\n";
}

void do_light_sensors() {
  if ((global_cnt & 0x07) != 0x07) {
    return;
  }
  left_light = analogRead(light_L_Pin);
  right_light = analogRead(light_R_Pin);
  if (left_light > LIGHT_TRIGGER || right_light > LIGHT_TRIGGER) {
    if (START_BY_LIGHT && !FOLLOW_LIGHT) {
      car_start = true;
      set_car_mode(CAR_FORWARD);
    } else if (car_start) {
      car_stop();
    }
    if (FOLLOW_LIGHT) {
      last_light_event = 32;
      if (left_light > right_light) {
        servo_target = LOOK_AHEAD + 80;
      } else {
        servo_target = LOOK_AHEAD - 80;
      }
    } else {
      if (last_light_event >= 0) {
        last_light_event--;
        if (last_light_event == 0) {
          servo_target = LOOK_AHEAD;
        }
      }
    }
  }
}

void car_forward() {
  matrix_what = MATRIX_FORWARD;
  if (!MOTOR_ACTIVE) {
    return;
  }
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, 220 - (speed + RIGHT_WHEEL_EXTRA));
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, 220 - (speed + LEFT_WHEEL_EXTRA));
}

void car_backward() {
  if(matrix_what != MATRIX_DIZZY) matrix_what = MATRIX_BACKWARD;
  if (!MOTOR_ACTIVE) {
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 80);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 80);
}

void car_left() {
  matrix_what = MATRIX_LEFT;
  car_cnt = 0;
  if (!MOTOR_ACTIVE) {
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 110);
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, 180);
}

void car_right() {
  matrix_what = MATRIX_RIGHT;
  car_cnt = 0;
  if (!MOTOR_ACTIVE) {
    return;
  }
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, 180);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 110);
}
void car_stop() {
  matrix_what = MATRIX_STOP;
  if (!MOTOR_ACTIVE) {
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 0);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 0);
}

void wait_for_start() {
  if (!START_BY_DISTANCE) {
    // start by distance is disabled
    matrix_what = MATRIX_STOP;
    return;
  }
  if ((global_cnt >> 9) & 1 == 1) {
    matrix_what = MATRIX_SCROLL_TEXT;
  } else {
    if ((global_cnt >> 5) & 1 == 1) {
      matrix_what = MATRIX_WAIT_LEFT;
    } else {
      matrix_what = MATRIX_WAIT_RIGHT;
    }
  }
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

void look(uint8_t direction) {
  servo.write(direction);
  delay(5);
  servo.write(direction);
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
  if (speed != old_speed && car_mode == CAR_FORWARD) {
    analogWrite(MR_PWM, 200 - speed);
    analogWrite(ML_PWM, 200 - speed);
    old_speed = speed;
  }
  if ((car_mode == CAR_LEFT || car_mode == CAR_RIGHT) && car_forced_turn) {
    if (car_cnt > left_right_time) {
      car_forced_turn = false;
      speed = forward_speed;
      analogWrite(MR_PWM, 220 - speed);
      analogWrite(ML_PWM, 220 - speed);
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
        car_mode = CAR_STOP;
        car_start = false;
        break;
    }
    irrecv.resume();
  }
}

void do_led() {
  if (((global_cnt & 0x07) == 0x07) && (led_blink != 0xff)) {
    led_blink = led_blink ^ 1;
    digitalWrite(LED_PIN, led_blink);
  }
}

void do_look_setup() {
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

void loop() {
  do_matrix();
  if (look_setup < LOOK_SETUP_TAKES) {
    do_look_setup();
  } else {
    do_distance();
    do_line_sensor();
    do_car_movement();
    do_light_sensors();
    do_led();
    do_servo();
    do_IR();
  }
  if(last_mode_change > 0) {
    last_mode_change--;
  }
  if(mode_wanted_cnt > 0) {
    mode_wanted_cnt--;
    if(mode_wanted_cnt == 0 && mode_wanted != 0xff) {
      Serial << "delayed mode: " << mode_wanted << "\n";
      car_mode = mode_wanted;
      mode_wanted = 0xff;
    }
  }
  global_cnt++;
  delay(10);
}
