#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>

typedef struct OutputPin {
  volatile uint8_t* DDR;
  volatile uint8_t* PORT;
  char PIN;
} out_pin_t;

typedef struct InputPin {
  volatile uint8_t* DDR;
  volatile uint8_t* PORT;
  volatile uint8_t* PINR;
  char PIN;
} in_pin_t;

typedef struct Keypad {
  out_pin_t* cols;
  uint8_t cols_no;

  in_pin_t* rows;
  uint8_t rows_no;
} keypad_t;

keypad_t make_keypad(out_pin_t* cols, uint8_t cols_no, in_pin_t* rows, uint8_t rows_no) {
  return (keypad_t){cols, cols_no, rows, rows_no};
}

void init_keypad(keypad_t* keypad) {
  for(uint8_t i = 0; i < keypad->cols_no; i++) {
	*keypad->cols[i].DDR |= _BV(keypad->cols[i].PIN);
	*keypad->cols[i].PORT |= _BV(keypad->cols[i].PIN);
  }

  for(uint8_t i = 0; i < keypad->rows_no; i++) {
	*keypad->rows[i].DDR &= ~_BV(keypad->rows[i].PIN);
	*keypad->rows[i].PORT |= _BV(keypad->rows[i].PIN);
  }
}

bool key_pressed(keypad_t* keypad, uint8_t col, uint8_t row) {
  *keypad->cols[col].PORT &= ~_BV(keypad->cols[col].PIN);
  // _delay_ms(10)
  bool status = (*keypad->rows[row].PINR & _BV(keypad->rows[row].PIN)) != _BV(keypad->rows[row].PIN);

  *keypad->cols[col].PORT |= ~_BV(keypad->cols[col].PIN);
  return status;
}

#endif
