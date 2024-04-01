#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <string.h>

// PINOUT https://www.teachmemicro.com/wp-content/uploads/2019/06/Arduino-Nano-pinout-3.jpg

#include "lcd.h"
#include "font.h"
#include "keypad.h"

#define COLS_NO 3
#define ROWS_NO 4

static dpin_t COLS[COLS_NO] = {7, 9, 5};
static dpin_t ROWS[ROWS_NO] = {8, 3, 4, 6};

const char cmap[COLS_NO*ROWS_NO] = {
  '#', '9', '6', '3',
  '0', '8', '5', '2',
  '*', '7', '4', '1',
};

#define TEXT_HEIGHT (DISPLAY_HEIGHT/8)
#define TEXT_WIDTH (DISPLAY_WIDTH/sizeof(FONT[0]))

#define MENU_ITEMS_NO 11

const char* MENU_ITEMS[MENU_ITEMS_NO] = {
  "TEST",
  "TEST2",
  "TEST3",
  "TEST4",
  "TEST5",
  "TEST6",
  "TEST7",
  "TEST8",
  "TEST9",
  "TEST10",
  "TEST11",
};

static uint8_t SELECTED = 0;
static uint8_t ORIGIN = 0;

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

static inline void draw_menu(void) {
  lcd_clear_buffer();

  char tmp[32];

  sprintf(tmp, "%i %i %i %i", ORIGIN, SELECTED, TEXT_WIDTH, TEXT_HEIGHT);

  lcd_gotoxy(TEXT_WIDTH - strlen(tmp), 0);
  lcd_puts(tmp);

  for(uint8_t i = ORIGIN; i < MIN(MENU_ITEMS_NO, ORIGIN + TEXT_HEIGHT); i++) {
	lcd_gotoxy(1, i - ORIGIN);
	lcd_puts(MENU_ITEMS[i]);
  }

  lcd_gotoxy(0, SELECTED - ORIGIN);
  lcd_putc('>');
}

static inline void update_origin(void) {
  if(SELECTED + 2 - ORIGIN >= TEXT_HEIGHT) {
  } else {
	ORIGIN = 0;
  }
}

static inline void inc_selected(void) {
  if(SELECTED >= MENU_ITEMS_NO - 1) {
	return;
  }

  SELECTED += 1;

  if(SELECTED + 2 - ORIGIN >= TEXT_HEIGHT &&
	 ORIGIN < MENU_ITEMS_NO - TEXT_HEIGHT) {
	ORIGIN += 1;

	draw_menu();

	return;
  }

  lcd_gotoxy(0, SELECTED - ORIGIN - 1);
  lcd_putc(' ');

  lcd_gotoxy(0, SELECTED - ORIGIN);
  lcd_putc('>');
}

static inline void dec_selected(void) {
  if(SELECTED <= 0) {
	return;
  }

  SELECTED -= 1;

  if(SELECTED <= ORIGIN + 2 && ORIGIN > 0) {
	ORIGIN -= 1;

	draw_menu();

	return;
  }

  lcd_gotoxy(0, SELECTED - ORIGIN + 1);
  lcd_putc(' ');

  lcd_gotoxy(0, SELECTED - ORIGIN);
  lcd_putc('>');
}

int main(void) {
  keypad_t keypad = make_keypad(COLS, COLS_NO, ROWS, ROWS_NO);
  init_keypad(&keypad);

  lcd_init(LCD_DISP_ON);

  update_origin();

  draw_menu();
  lcd_display();

  uint16_t last_mask = 0;
  uint16_t current_mask, diff_mask;

  bool changed;

  while(true) {
	_delay_ms(10);

	changed = true;

	current_mask = key_scan16(&keypad);
	diff_mask = ~last_mask & current_mask;

	char pressed = '\0';
	for(uint8_t i = 0; i < COLS_NO*ROWS_NO; i++) {
	  if((diff_mask >> i) & 1) {
		pressed = cmap[i];
		break;
	  }
	}

	switch(pressed) {
	case '2':
	  dec_selected();
	  break;
	case '8':
	  inc_selected();
	  break;

	default:
	  changed = false;
	}

	if(changed) {
	  lcd_display();
	}

	last_mask = current_mask;
  }

  /* uint16_t last_mask = 0; */
  /* uint16_t current_mask, diff_mask; */

  /* uint8_t x = 0; */
  /* uint8_t y = 0; */

  /* while(true) { */
  /*	_delay_ms(16); */

  /*	current_mask = key_scan16(&keypad); */
  /*	diff_mask = ~last_mask & current_mask; */

  /*	for(uint8_t i = 0; i < COLS_NO*ROWS_NO; i++) { */
  /*	  if((diff_mask >> i) & 1) { */
  /*		lcd_gotoxy(x, y); */
  /*		lcd_putc(cmap[i]); */

  /*		x += 1; */
  /*	  } */

  /*	} */

  /*	last_mask = current_mask; */
  /*	lcd_display(); */
  /* } */

  /* bool mask[ROWS_NO*COLS_NO]; */
  /* char str[32]; */

  /* while(true) { */
  /*	_delay_ms(10); */

  /*	key_scan(&keypad, mask); */

  /*	for(int col = 0; col < COLS_NO; col++) { */
  /*	  for(int row = 0; row < ROWS_NO; row++) { */
  /*		lcd_gotoxy(col, row); */
  /*		lcd_puts(mask[col*ROWS_NO + row] ? " " : "#"); */
  /*	  } */
  /*	} */

  /*	lcd_gotoxy(5, 0); */

  /*	uint8_t index = key_get_first(&keypad); */
  /*	sprintf(str, "%i  ", index); */
  /*	lcd_puts(str); */

  /* } */

  /* while(true) { */
  /*	_delay_ms(16); */
  /*	lcd_gotoxy(0, 0); */

  /*	if(key_pressed(&keypad, 0, 0)) { */
  /*	  lcd_puts("ON "); */
  /*	} else { */
  /*	  lcd_puts("OFF "); */
  /*	} */
  /* } */

  /* int no = 0; */
  /* int col = 0; */
  /* int row = 0; */

  /* char str[32]; */

  /* while(true) { */
  /*	_delay_ms(100); */
  /*	lcd_gotoxy(0, 0); */

  /*	if(key_pressed(&keypad, col, row)) { */
  /*	  lcd_puts("ON "); */
  /*	} else { */
  /*	  lcd_puts("OFF"); */
  /*	} */

  /*	lcd_gotoxy(0, 1); */
  /*	sprintf(str, "%i, %i %i ", col, row, no); */
  /*	lcd_puts(str); */

  /*	no += 1; */

  /*	if(no == 10) { */
  /*	  col += 1; */
  /*	  if(col == COLS_NO) { */
  /*		row += 1; */
  /*		col = 0; */
  /*		if(row == ROWS_NO) { */
  /*		  row = 0; */
  /*		} */
  /*	  } */
  /*	  no = 0; */
  /*	} */

  /* } */


  /* char str[32]; */
  /* while(true) { */
  /*	int col, row; */
  /*	for(col = 0; col < COLS_NO; col++) { */
  /*	  *COLS[col].PORT &= ~_BV(COLS[col].PIN); */

  /*	  for(row = 0; row < ROWS_NO; row++) { */
  /*		_delay_ms(1000); */
  /*		bool status = (*ROWS[row].PINR & _BV(ROWS[row].PIN)) == _BV(ROWS[row].PIN); */

  /*		lcd_gotoxy(0, 1); */

  /*		sprintf(str, "%i, %i %s", col, row, status ? "true " : "false"); */
  /*		lcd_puts(str); */

  /*		if(!status) { */
  /*		  goto sucess; */
  /*		} */
  /*	  } */
  /*	  *COLS[col].PORT |= _BV(COLS[col].PIN); */
  /*	} */

  /* fail: */
  /*	lcd_gotoxy(0, 0); */
  /*	lcd_puts("NONE"); */

  /*	goto end; */
  /* sucess: */
  /*	lcd_gotoxy(0, 0); */

  /*	sprintf(str, "%i, %i", col, row); */

  /*	lcd_puts(str); */
  /* end: */
  /*}*/

  /*   lcd_gotoxy(0,2);          // set cursor to first column at line 3 */
  /*   lcd_puts_p(PSTR("String from flash"));  // puts string form flash to display (TEXTMODE) or buffer (GRAPHICMODE) */
  /* #if defined GRAPHICMODE */
  /*   lcd_drawCircle(64,32,7,WHITE); // draw circle to buffer white lines */
  /*   lcd_display();                 // send buffer to display */
  /* #endif */

}
