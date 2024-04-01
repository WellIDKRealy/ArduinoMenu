#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>

#include "pinout.h"

typedef struct Keypad {
  dpin_t* cols;
  uint8_t cols_no;

  dpin_t* rows;
  uint8_t rows_no;
} keypad_t;

keypad_t make_keypad(dpin_t* cols, uint8_t cols_no, dpin_t* rows, uint8_t rows_no);
void init_keypad(keypad_t* keypad);
bool key_pressed(keypad_t* keypad, uint8_t col, uint8_t row);
void key_scan(keypad_t* keypad, bool* mask);
uint16_t key_scan16(keypad_t* keypad);
uint8_t key_get_first(keypad_t* keypad);

#endif
