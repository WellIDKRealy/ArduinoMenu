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


enum MenuItemType {
  MenuItemType,
  MenuProgramType,
  MenuType,
};

typedef union MenuItemU {
  struct MenuItem* menu_item;
  struct MenuProgram* menu_program;
  struct Menu* menu;
} menu_item_union_t;

typedef struct MenuItem {
  enum MenuItemType type;
  char* name;
} menu_item_t;

typedef struct MenuProgram {
  enum MenuItemType type;
  char* name;

  void (*program_f)();
} menu_program_t;

typedef struct Menu {
  enum MenuItemType type;
  char* name;

  uint8_t origin;
  uint8_t selected;

  uint8_t size;
  menu_item_union_t items[];
} menu_t;


typedef struct MenuStack {
  struct MenuStack* back;
  menu_t* menu;
} menu_stack_t;


static menu_t GAMES_MENU = {
  MenuType,
  "Games",
  0,
  0,
  8,
  {
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game1"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game2"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game3"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game4"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game5"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game6"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game7"},
	(menu_item_union_t)&(menu_item_t){MenuItemType, "Game8"},
  }
};

static menu_t DEFAULT_MENU = {
  MenuType,
  "Default",
  0,
  0,
  1,
  {
	(menu_item_union_t)&GAMES_MENU,
  }
};

static menu_t* SELECTED_MENU = &DEFAULT_MENU;
static menu_stack_t* MENU_STACK = NULL;

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
	lcd_puts(menu->items[i].menu_item->name);
  }

  draw_selected(menu);
}

static inline void inc_selected(menu_t* menu) {
  if(menu->selected + 1 >= menu->size) {
	return;
  }

  clear_selected(menu);

  menu->selected += 1;

  if(menu->selected - menu->origin + 2 >= TEXT_HEIGHT - 1 && menu->size - menu->origin > TEXT_HEIGHT - 1) {
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
	  if(MENU_STACK == NULL) {
		break;
	  }

	  SELECTED_MENU = MENU_STACK->menu;

	  menu_stack_t* back = MENU_STACK->back;
	  free(MENU_STACK);
	  MENU_STACK = back;

	  draw_menu(SELECTED_MENU);

	  break;
	case '6':
	  menu_item_union_t val = SELECTED_MENU->items[SELECTED_MENU->selected];

	  switch(val.menu_item->type) {
	  case MenuType:
		MENU_STACK = &(menu_stack_t){MENU_STACK, SELECTED_MENU};
		SELECTED_MENU = val.menu;

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
}
