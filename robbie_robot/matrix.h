#ifndef __GLOBAL__
#include "global.h"
#endif

#ifndef __MATRIX__

#define __MATRIX__

#include <Adafruit_NeoPixel.h>

#define SCL_Pin A5            //set a pin of clock to A5
#define SDA_Pin A4            //set a data pin to A4
#define SWITCH_DISPLAY_SHR 7  // global_cnt >> this value -> switch display mode

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
#define MATRIX_BACKWARD 12

uint8_t matrix_what = 4;  // set to display something else (4 is blank)
uint8_t old_matrix_what = 0xff;

uint16_t old_matrix = 0xffff;
uint16_t scroll_pos;
uint16_t scrolled_for;
uint16_t scroll_feed_idx = 0;
boolean scroll_feed_repeat = false;
#define SCROLL_FEED_SLOWDOWN 0x1

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
unsigned char slash[] = { 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x03, 0x00 };

unsigned char scroll_img[16] = { 0 };

void setup_matrix() {
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
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

void do_matrix() {
  switch (matrix_what) {
    case MATRIX_FORWARD: matrix_display(arrow_forward); break;
    case MATRIX_LEFT: matrix_display(arrow_left); break;
    case MATRIX_RIGHT: matrix_display(arrow_right); break;
    case MATRIX_STOP: matrix_display(stop); break;
    case MATRIX_CLEAR: clear_matrix(); break;
    case MATRIX_DIZZY: show_dizzy(); break;
    case MATRIX_LINE_SENSORS: clear_matrix(); break; // show_line_sensors();
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
      case MATRIX_BACKWARD: matrix_display(arrow_backward); break;
    default: show_dizzy(); break;
  }
  old_matrix = matrix_what;
}

#endif