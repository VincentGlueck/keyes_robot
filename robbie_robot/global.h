#ifndef __GLOBAL__

#define __GLOBAL__

uint16_t global_cnt = 0;

template<typename T>
Print& operator<<(Print& printer, T value) {
  printer.print(value);
  return printer;
}

uint8_t za = random(256);
uint8_t zb = random(256);
uint8_t zc = random(256);
uint8_t zx = random(256);

uint8_t rnd() {
  zx++;
  za = (za ^ zc ^ zx);
  zb = (zb + za);
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}
#endif