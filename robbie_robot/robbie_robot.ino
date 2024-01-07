#include <Board_Identify.h>
#include <IRremote.h>
#include <Servo.h>

/****************************************************************
 * DO NOT SET TO true UNTIL YOU DISCONNECTED ROBOT FROM USB !!! *
****************************************************************/

// ##################  THIS ONE: ##################

#define MOTOR_ACTIVE false

// ##### MIGHT DAMAGE YOUR TOY IF SET TO TRUE #####


// show distance on console?
#define DISTANCE_CONSOLE false
#define SHOW_DISTANCE_MASK 0x7F  // how often will it show up
// show IR codes received?
#define PRINT_IR_CODES true
#define PRINT_IR_MANUFACTURER false  // combine true with PRINT_IR_CODES true, ok?
// should engines start by holding hand near distance sensor?
#define START_BY_DISTANCE true
// should engines start by light (by phone) directly spotted at light sensors?
#define START_BY_LIGHT false
// "eyes" follow light, should not be combined with START_BY_LIGHT true
#define FOLLOW_LIGHT true

// LED
#define LED_PIN 9

// ultrasonic
#define TRIG_PIN 13
#define ECHO_PIN 12
#define LOOK_SETUP_TAKES 5  // init loop count of ultrasonic

// matrix display
#define SCL_Pin A5            //set a pin of clock to A5
#define SDA_Pin A4            //set a data pin to A4
#define SWITCH_DISPLAY_SHR 7  // global_cnt >> this value -> switch display mode

int matrix_what = 4;  // set to display something else (4 is blank)
// 0 forward, 1 left, 2 right, 3 stop, 4 clear, 5 dizzy, 6 line sensors, 7 wait_left, 8 wait_right, 9 hour_glass, 10 scroll


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
int servo_angle = 0;
int servo_target = 45;

// motor control
#define ML_Ctrl 4  // direction left motor
#define ML_PWM 6   // PWM control left motor
#define MR_Ctrl 2  // direction right motor
#define MR_PWM 5   // PWM right motor

// IR control
#define IR_PIN 3

// IR key codes, you may want to modify these
#define IR_1 FF6897
#define IR_2 FF9867
#define IR_3 FFB04F
#define IR_4 FF30CF
#define IR_5 FF18E7
#define IR_6 FF7A85
#define IR_7 FF10EF
#define IR_8 FF38C7
#define IR_9 FF5AA5
#define IR_0 FF4AB5
#define IR_STAR FF42BD
#define IR_HASH FF52AD
#define IR_UP FF629D
#define IR_LEFT FF22DD
#define IR_RIGHT FFC23D
#define IR_DOWN FFA857
#define IR_OK FF02FD

#define IR_SKIP 0xFFFFFFFF

IRrecv irrecv(IR_PIN);
decode_results results;
long ir_rec;  //Used to store the received infrared values

// matrix images
unsigned char arrow_left[] = { 0x00, 0x00, 0x00, 0x18, 0x18, 0x3c, 0x66, 0x00, 0x18, 0x18, 0x7c, 0x66, 0x00, 0x00, 0x00, 0x00 };
unsigned char arrow_right[] = { 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0x18, 0x18, 0x00, 0x66, 0x3c, 0x18, 0x18, 0x00, 0x00, 0x00 };
unsigned char arrow_forward[] = { 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0e, 0x07, 0xff, 0xff, 0x07, 0x0e, 0x1c, 0x18, 0x00, 0x00, 0x00 };
unsigned char skull[] = { 0x00, 0x00, 0x00, 0x00, 0x3c, 0x72, 0xd7, 0xdf, 0xdf, 0xd7, 0x72, 0x3c, 0x00, 0x00, 0x00, 0x00 };
unsigned char stop[] = { 0x00, 0x00, 0x00, 0x00, 0x3c, 0x72, 0xd7, 0xdf, 0xdf, 0xd7, 0x72, 0x3c, 0x00, 0x00, 0x00, 0x00 };
unsigned char line[] = { 0x00, 0x0f, 0x08, 0x00, 0x0f, 0x00, 0x0f, 0x02, 0x04, 0x0f, 0x00, 0x0f, 0x0b, 0x09, 0x00, 0x00 };
unsigned char wait_left[] = { 0x18, 0x24, 0x24, 0x18, 0x00, 0x70, 0x30, 0x50, 0x80, 0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x18 };
unsigned char wait_right[] = { 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x80, 0x40, 0x28, 0x18, 0x38, 0x00, 0x18, 0x24, 0x24, 0x18 };
unsigned char hour_glass[] = { 0x00, 0x00, 0x00, 0x00, 0x81, 0xc3, 0xe7, 0xff, 0xe7, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char a_letter[] = { 0xe0, 0x78, 0x2e, 0x21, 0x2e, 0x78, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char scroll_img[16] = { 0 };

// matrix for line sensors
unsigned char line_white[] = { 0x60, 0x90, 0x90, 0x60 };
unsigned char line_black[] = { 0x60, 0xf0, 0xf0, 0x60 };

uint8_t za, zb, zc, zx, counter;  // fast random generator, see rnd()
uint16_t global_cnt = 0;
uint16_t old_slow = 0xffff;
uint16_t scrolled_for;

// line sensors
int L_val, M_val, R_val;
int L_old, M_old, R_old;

// light sensors
int left_light, right_light, last_light_event;

// vehicle's initial speed (quite slow)
uint8_t speed = 20;
boolean car_start = false;
boolean might_drive = false;
boolean look_for_directions = true;
int look_around = 0;
uint8_t look_setup = 0;
uint16_t look_cnt = 128;


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

  Serial.println("Robbie is waking up...");
  Serial.print(F("Heart: "));
  Serial.print(BoardIdentify::model);
  Serial.print(" ");
  Serial.println(BoardIdentify::mcu);
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
}

void do_matrix() {
  switch (matrix_what) {
    case 0: matrix_display(arrow_forward); break;
    case 1: matrix_display(arrow_left); break;
    case 2: matrix_display(arrow_right); break;
    case 3: matrix_display(stop); break;
    case 4: clear_matrix(); break;
    case 5: show_dizzy(); break;
    case 6: show_line_sensors(); break;
    case 7: matrix_display(wait_left); break;
    case 8: matrix_display(wait_right); break;
    case 9: matrix_display(hour_glass); break;
    case 10:
      {
        if (old_slow != matrix_what) {
          clear_scroll_img();
        }
        show_scroll();
        break;
      }
    default: show_dizzy(); break;
  }
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

void measure_distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  //Give at least 10us high level trigger
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  boolean near_by = distance >= 2 && distance <= 10;
  if (near_by) {
    might_drive = false;
    if (START_BY_DISTANCE) {
      car_start = true;
      matrix_what = 0;
    }
    digitalWrite(LED_PIN, HIGH);
  } else {
    might_drive = true;
    digitalWrite(LED_PIN, LOW);
  }
  if (DISTANCE_CONSOLE && (global_cnt & SHOW_DISTANCE_MASK) == SHOW_DISTANCE_MASK) {
    // europe, inch is similar
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print(" cm");
    if (near_by) {
      Serial.print("  --> crashing");
    }
    Serial.println();
  }
}

void do_servo() {
  if (servo_angle > servo_target) {
    servo_angle--;
    servo.write(servo_angle);
  } else if (servo_angle < servo_target) {
    servo_angle++;
    servo.write(servo_angle);
  }
}

void do_light_sensors() {
  left_light = analogRead(light_L_Pin);
  right_light = analogRead(light_R_Pin);
  if (left_light > LIGHT_TRIGGER || right_light > LIGHT_TRIGGER) {
    if (START_BY_LIGHT && !FOLLOW_LIGHT) {
      car_start = true;
    }
    if (FOLLOW_LIGHT) {
      last_light_event = 32;
      if (left_light > right_light) {
        Serial.print("left_light_value wins: ");
        Serial.println(left_light);
        servo_target = 170;
      } else {
        Serial.print("right_light_value wins: ");
        Serial.println(right_light);
        servo_target = 10;
      }
    } else {
      if (last_light_event >= 0) {
        last_light_event--;
        if (last_light_event == 0) {
          servo_target = 90;
          Serial.println("look straight");
        }
      }
    }
  }
}

void motor_off_msg(uint8_t msg) {
  if (msg == last_msg) {
    return;
  }
  if ((global_cnt & 0x3F) == 0x3F) {
    Serial.print("Motor off :: ");
    switch (msg) {
      case 0: Serial.println("drive forward"); break;
      case 1: Serial.println("drive left"); break;
      case 2: Serial.println("drive right"); break;
      case 3: Serial.println("!!! STOP MOTOR !!!"); break;
      case 4: Serial.println("drive backward"); break;
    }
  }
  last_msg = msg;
}

void car_forward() {
  if (!MOTOR_ACTIVE) {
    motor_off_msg(0);
    return;
  }
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, speed);
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, speed);
}

void car_backward() {
  if (!MOTOR_ACTIVE) {
    motor_off_msg(4);
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, speed);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, speed);
}

void car_left() {
  if (!MOTOR_ACTIVE) {
    motor_off_msg(1);
    return;
  }
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, speed);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, speed >> 2);
}
void car_right() {
  if (!MOTOR_ACTIVE) {
    motor_off_msg(2);
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, speed >> 2);
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, speed);
}
void car_stop() {
  if (!MOTOR_ACTIVE) {
    motor_off_msg(3);
    return;
  }
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 0);
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 0);
}

void decide_direction() {
}

void do_car_movement() {
  if (!car_start) {
    // wait for start
    if (!START_BY_DISTANCE) {
      // start by distance is disabled :-(
      matrix_what = 3;
    }
    if ((global_cnt >> 8) & 1 == 1) {
      matrix_what = 9;
    } else {
      if ((global_cnt >> 4) & 1 == 1) {
        matrix_what = 7;
      } else {
        matrix_what = 8;
      }
    }
    return;
  }
  if (look_for_directions) {
    car_stop();
    if (look_cnt <= 0) {
      Serial.print("Finding directions :: ");
      Serial.print(look_around);
      Serial.print(" -- cnt: ");
      Serial.println(look_cnt);
      switch (look_around) {
        case 0: look(170); break;
        case 1: look(10); break;
        case 2: look(90); decide_direction();
        default:
          {
            look_for_directions = false;
          }
      }
    } else {
      look_cnt--;
      if (look_cnt <= 0) {
        look_cnt = 128;
        look_around++;
      }
    }
    return;
  }
  if (might_drive) {
    matrix_what = 0;
    car_forward();
  } else {
    car_stop();
    matrix_what = 3;
    might_drive = false;
    look_for_directions = true;
  }
}

void look(uint8_t direction) {
  servo.write(direction);
}

void do_look_setup() {
  matrix_what = 5;
  if ((global_cnt & 0x1F) == 0x1F) {
    switch (look_setup) {
      case 0: look(90); break;
      case 1: look(170); break;  // look left
      case 2: look(90); break;
      case 3: look(10); break;  //look right
      case 4:
        look(90);
        Serial.println("Robbie Robot is ready...");
        break;
      default: break;
    }
    look_setup++;
  }
}

void do_IR() {
  if (irrecv.decode(&results)) {
    ir_rec = results.value;
    if (PRINT_IR_CODES && PRINT_IR_MANUFACTURER) {
      String type = "UNKNOWN";
      String typelist[14] = { "UNKNOWN", "NEC", "SONY", "RC5", "RC6", "DISH", "SHARP", "PANASONIC", "JVC", "SANYO", "MITSUBISHI", "SAMSUNG", "LG", "WHYNTER" };
      if (results.decode_type >= 1 && results.decode_type <= 13) {
        type = typelist[results.decode_type];
        Serial.print("IR TYPE: ");
        Serial.print(type);
        Serial.print(", ");
      }
    }
    if (PRINT_IR_CODES || PRINT_IR_MANUFACTURER) {
      if (ir_rec != IR_SKIP) {
        Serial.print("ir_rec: ");
        Serial.println(ir_rec, HEX);
      }
    }
    irrecv.resume();
  }
}

void loop() {
  measure_distance();
  do_line_sensor();
  do_matrix();
  if ((global_cnt & 0x07) == 0x07) {
    do_light_sensors();
  }
  if (look_setup < LOOK_SETUP_TAKES) {
    do_look_setup();
  } else {
    do_car_movement();
  }
  do_servo();
  do_IR();
  global_cnt++;
  delay(10);
}
