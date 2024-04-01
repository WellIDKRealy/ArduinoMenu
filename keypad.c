#include "keypad.h"

keypad_t make_keypad(dpin_t* cols, uint8_t cols_no, dpin_t* rows, uint8_t rows_no) {
  return (keypad_t){cols, cols_no, rows, rows_no};
}

void init_keypad(keypad_t* keypad) {
  for(uint8_t i = 0; i < keypad->cols_no; i++) {
	pin_direction(keypad->cols[i], true);
	pin_pullup(keypad->cols[i], true);
  }

  for(uint8_t i = 0; i < keypad->rows_no; i++) {
	pin_direction(keypad->rows[i], false);
	pin_pullup(keypad->rows[i], true);
  }
}

bool key_pressed(keypad_t* keypad, uint8_t col, uint8_t row) {
  pin_pullup(keypad->cols[col], false);

  bool status = !pin_read(keypad->rows[row]);

  pin_pullup(keypad->cols[col], true);
  return status;
}

// mask is 2d array of size rows_no x cols_no
void key_scan(keypad_t* keypad, bool* mask) {
  for(uint8_t col = 0; col < keypad->cols_no; col++) {
	pin_pullup(keypad->cols[col], false);
	for(uint8_t row = 0; row < keypad->rows_no; row++) {
	  mask[row*keypad->cols_no + col] = !pin_read(keypad->rows[row]);
	}
	pin_pullup(keypad->cols[col], true);
  }
}
