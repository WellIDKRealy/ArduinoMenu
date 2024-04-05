#include <util/delay.h>

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

// mask is 2d array of size cols_no x rows_no
void key_scan(keypad_t* keypad, bool* mask) {
  for(uint8_t col = 0; col < keypad->cols_no; col++) {
	pin_pullup(keypad->cols[col], false);
	for(uint8_t row = 0; row < keypad->rows_no; row++) {
	  mask[col*keypad->rows_no + row] = !pin_read(keypad->rows[row]);
	}
	pin_pullup(keypad->cols[col], true);
  }
}

// for keypads with less than 17 options
uint16_t key_scan16(keypad_t* keypad) {
  uint16_t result = 0;

  for(uint8_t col = 0; col < keypad->cols_no; col++) {
	pin_pullup(keypad->cols[col], false);
	for(uint8_t row = 0; row < keypad->rows_no; row++) {
	  result = result << 1;
	  result |= !pin_read(keypad->rows[row]);
	}
	pin_pullup(keypad->cols[col], true);
  }

  return result;
}

uint8_t key_get_first(keypad_t* keypad) {
  for(uint8_t col = 0; col < keypad->cols_no; col++) {
	pin_pullup(keypad->cols[col], false);
	for(uint8_t row = 0; row < keypad->rows_no; row++) {
	  if(!pin_read(keypad->rows[row])) {
		pin_pullup(keypad->cols[col], true);
		return col*keypad->rows_no + row;
	  }
	}
	pin_pullup(keypad->cols[col], true);
  }

  return 255;
}

char key_get_first_char_diff(keypad_t* keypad, const char* cmap, uint16_t current_mask, uint16_t last_mask) {
  uint16_t diff_mask = ~last_mask & current_mask;

  char pressed = '\0';
  for(uint8_t i = 0; i < keypad->rows_no*keypad->cols_no; i++) {
	if((diff_mask >> i) & 1) {
	  pressed = cmap[i];
	  break;
	}
  }

  return pressed;

}
