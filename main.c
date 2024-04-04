#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <string.h>

// PINOUT https://www.teachmemicro.com/wp-content/uploads/2019/06/Arduino-Nano-pinout-3.jpg

#include "lcd.h"
#include "font.h"
#include "keypad.h"


#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))


#define TEXT_HEIGHT (DISPLAY_HEIGHT/8)
#define TEXT_WIDTH (DISPLAY_WIDTH/sizeof(FONT[0]))

#define COLS_NO 3
#define ROWS_NO 4


static dpin_t COLS[COLS_NO] = {7, 9, 5};
static dpin_t ROWS[ROWS_NO] = {8, 3, 4, 6};

const char cmap[COLS_NO*ROWS_NO] = {
  '#', '9', '6', '3',
  '0', '8', '5', '2',
  '*', '7', '4', '1',
};


enum MenuItemT {Empty = 0, Program = 1, Menu = 2};

typedef struct MenuEmpty {
  // Shared
  enum MenuItemT type;
  char* name;
} menu_empty_t;


typedef struct MenuProgram {
  // Shared
  enum MenuItemT type;
  char* name;

  void (*program_f)();
} menu_program_t;


typedef struct Menu {
  // Shared
  enum MenuItemT type;
  char* name;

  struct Menu* father;

  uint8_t origin;
  uint8_t selected;

  uint8_t size;
  union MenuItem items[];

} menu_t;

typedef union MenuItem {
  struct MenuEmpty empty;
  struct MenuProgram program;
  struct Menu menu;
} menu_item_t;


void set_fathers(menu_t* menu) {
  for(uint8_t i = 0; i < menu->size; i++) {
	if(menu->items[i].type == Menu) {
	  menu_t* val = (menu_t*)&menu->items[i];
	  val->father = menu;
	  set_fathers(val);
	}
  }
}

static menu_t GAMES_MENU = {
  Menu,
  "Games",
  NULL,
  0,
  0,
  8,
  {
	(menu_empty_t){Empty, "GAME1"},
	(menu_empty_t){Empty, "GAME2"},
	(menu_empty_t){Empty, "GAME3"},
	(menu_empty_t){Empty, "GAME4"},
	(menu_empty_t){Empty, "GAME5"},
	(menu_empty_t){Empty, "GAME6"},
	(menu_empty_t){Empty, "GAME7"},
	(menu_empty_t){Empty, "GAME8"},
  }
};

static menu_t DEFAULT_MENU = {
  Menu,
  "Default",
  NULL,
  0,
  0,
  8,
  {
	GAMES_MENU,
	(menu_empty_t){Empty, "TEST2"},
	(menu_empty_t){Empty, "TEST3"},
	(menu_empty_t){Empty, "TEST4"},
	(menu_empty_t){Empty, "TEST5"},
	(menu_empty_t){Empty, "TEST6"},
	(menu_empty_t){Empty, "TEST7"},
	(menu_empty_t){Empty, "TEST8"},
  }
};


static menu_t* SELECTED_MENU = &DEFAULT_MENU;

static inline void clear_selected(menu_t* menu) {
  lcd_gotoxy(0, menu->selected - menu->origin + 1);
  lcd_putc(' ');
}

static inline void draw_selected(menu_t* menu) {
  lcd_gotoxy(0, menu->selected - menu->origin + 1);
  lcd_putc('>');
}

static inline void draw_menu(menu_t* menu) {
  lcd_clear_buffer();

  // header
  char header[TEXT_WIDTH];
  sprintf(header, "menu: %s", menu->name);

  lcd_gotoxy(0, 0);
  lcd_puts(header);

  for(uint8_t i = menu->origin; i < MIN(menu->size, menu->origin + TEXT_HEIGHT - 1); i++) {
	lcd_gotoxy(1, i - menu->origin + 1);
	lcd_puts(menu->items[i].name);
  }

  draw_selected(menu);
}

static inline void inc_selected(menu_t* menu) {
  if(menu->selected + 1 >= menu->size) {
	return;
  }

  clear_selected(menu);

  menu->selected += 1;

  if(menu->selected - menu->origin + 2 >= TEXT_HEIGHT - 1) {
	menu->origin += 1;

	draw_menu(menu);
	return;
  }

  draw_selected(menu);
}

static inline void dec_selected(menu_t* menu) {
  if(menu->selected <= 0) {
	return;
  }

  clear_selected(menu);

  menu->selected -= 1;

  if(menu->selected - menu->origin < 2 && menu->origin > 0) {
	menu->origin -= 1;

	draw_menu(menu);
	return;
  }

  draw_selected(menu);
}


int main(void) {
  // idk if there is better solution
  set_fathers(&DEFAULT_MENU);

  keypad_t keypad = make_keypad(COLS, COLS_NO, ROWS, ROWS_NO);
  init_keypad(&keypad);

  lcd_init(LCD_DISP_ON);

  draw_menu(SELECTED_MENU);
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
	  dec_selected(SELECTED_MENU);
	  break;
	case '8':
	  inc_selected(SELECTED_MENU);
	  break;

	case '4':
	  if(SELECTED_MENU->father == NULL) {
		break;
	  }
	  SELECTED_MENU = SELECTED_MENU->father;
	  draw_menu(SELECTED_MENU);
	  break;
	case '6':
	  menu_item_t val = SELECTED_MENU->items[SELECTED_MENU->selected];

	  switch(val.type) {
	  case Menu:
		SELECTED_MENU = (menu_t*)&val;

		draw_menu(SELECTED_MENU);
		break;

	  default:
		break;
	  }
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
