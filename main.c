#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// PINOUT https://www.teachmemicro.com/wp-content/uploads/2019/06/Arduino-Nano-pinout-3.jpg

#include "lcd.h"
#include "font.h"
#include "keypad.h"
#include "menu.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))


#define TEXT_HEIGHT (DISPLAY_HEIGHT/8)
#define TEXT_WIDTH (DISPLAY_WIDTH/sizeof(FONT[0]))

#define COLS_NO 3
#define ROWS_NO 4


static dpin_t COLS[COLS_NO] = {7, 9, 5};
static dpin_t ROWS[ROWS_NO] = {8, 3, 4, 6};

static keypad_t keypad;

static uint16_t last_mask = 0;
static uint16_t current_mask;

const char cmap[COLS_NO*ROWS_NO] = {
  '#', '9', '6', '3',
  '0', '8', '5', '2',
  '*', '7', '4', '1',
};


enum Direction { Up, Down, Left, Right, None };

enum Direction snake_direction = Right;

enum Direction movement_map[TEXT_WIDTH*TEXT_HEIGHT];

static uint8_t head_x, head_y;
static uint8_t tail_x, tail_y;
static uint8_t apple_x, apple_y;

void snake(void) {
  lcd_clear_buffer();

  head_x = TEXT_WIDTH/2;
  head_y = TEXT_HEIGHT/2;

  tail_x = TEXT_WIDTH/2;
  tail_y = TEXT_HEIGHT/2;

  uint8_t snake_size = 1;

  apple_x = rand() % TEXT_WIDTH;
  apple_y = rand() % TEXT_HEIGHT;

  lcd_gotoxy(apple_x, apple_y);
  lcd_putc('O');

  while(true) {
	char pressed = key_get_first_char_diff(&keypad, cmap, current_mask, last_mask);
	last_mask = current_mask;

	switch(pressed) {
	case '2':
	  if(snake_direction != Down)
		snake_direction = Up;
	  break;
	case '5':
	case '8':
	  if(snake_direction != Up)
		snake_direction = Down;
	  break;
	case '4':
	  if(snake_direction != Right)
		snake_direction = Left;
	  break;
	case '6':
	  if(snake_direction != Left)
		snake_direction = Right;
	  break;
	}

	movement_map[head_y*TEXT_WIDTH + head_x] = snake_direction;

	enum Direction snake_tail_dir = movement_map[tail_y*TEXT_WIDTH + tail_x];
	movement_map[tail_y*TEXT_WIDTH + tail_x] = None;

	switch(snake_direction) {
	case Up:
	  if(head_y > 0) {
		head_y -= 1;
		break;
	  }
	  goto snake_lost;
	case Down:
	  if(head_y < TEXT_HEIGHT - 1) {
		head_y += 1;
		break;
	  }
	  goto snake_lost;
	case Left:
	  if(head_x > 0) {
		head_x -= 1;
		break;
	  }
	  goto snake_lost;
	case Right:
	  if(head_x < TEXT_WIDTH - 1) {
		head_x += 1;
		break;
	  }
	  goto snake_lost;
	case None:
	  /* Will not ever happen */
	  break;
	}

	if(head_x == apple_x && head_y == apple_y) {
	  snake_size += 1;

	  if(snake_size == TEXT_WIDTH*TEXT_HEIGHT) {
		goto snake_won;
	  }

	  apple_x = rand() % TEXT_WIDTH;
	  apple_y = rand() % TEXT_HEIGHT;

	  lcd_gotoxy(apple_x, apple_y);
	  lcd_putc('O');

	  movement_map[tail_y*TEXT_WIDTH + tail_x] = snake_tail_dir;
	  snake_tail_dir = None;
	}

	/* if(movement_map[head_y*TEXT_WIDTH + head_x] != None) { */
	/*   goto snake_lost; */
	/* } */

	lcd_gotoxy(tail_x, tail_y);
	lcd_putc(' ');

	switch(snake_tail_dir) {
	case Up:
	  tail_y -= 1;
	  break;
	case Down:
	  tail_y += 1;
	  break;
	case Left:
	  tail_x -= 1;
	  break;
	case Right:
	  tail_x += 1;
	  break;
	case None:
	  /* Will not ever happen */
	  break;
	}

	lcd_gotoxy(head_x, head_y);
	switch(snake_direction) {
	case Up:
	  lcd_putc('^');
	  break;
	case Down:
	  lcd_putc('V');
	  break;
	case Left:
	  lcd_putc('<');
	  break;
	case Right:
	  lcd_putc('>');
	  break;
	case None:
	  /* Will not ever happen */
	  break;
	}

	lcd_display();

	current_mask = 0;
	for(uint8_t i = 0; i < 10; i++) {
	  current_mask |= key_scan16(&keypad);
	  _delay_ms(10);
	}
  }

 snake_lost:
  lcd_clear_buffer();

  lcd_gotoxy(TEXT_WIDTH/2 - 4, TEXT_HEIGHT/2);
  lcd_puts("YOU LOST");

  lcd_display();

  _delay_ms(1000);

  return;

 snake_won:
  lcd_clear_buffer();

  lcd_gotoxy(TEXT_WIDTH/2 - 4, TEXT_HEIGHT/2);
  lcd_puts("YOU WON");

  lcd_display();

  _delay_ms(1000);

  return;
}

void hello_world(void) {
  lcd_clear_buffer();

  lcd_puts("Hello World");
  lcd_display();

  _delay_ms(1000);
}

#define RECT_SIZE 20

#define ROTATED_X(x, y, r_x, r_y) (x)*(r_x) - (y)*(r_y)
#define ROTATED_Y(x, y, r_x, r_y) (x)*(r_y) + (y)*(r_x)

void square_disp(void) {
  float center_x = DISPLAY_WIDTH/2;
  float center_y = DISPLAY_HEIGHT/2;

  float rotate_x, rotate_y;

  float S_X[4] = {-RECT_SIZE, RECT_SIZE, RECT_SIZE, -RECT_SIZE};
  float S_Y[4] = {-RECT_SIZE, -RECT_SIZE, RECT_SIZE, RECT_SIZE};

  float N_X[4];
  float N_Y[4];

  for(uint16_t i = 0; i < 10000; i++) {
	if((key_scan16(&keypad) != 0) && i > 10) {
	  return;
	}

	lcd_clear_buffer();

	rotate_x = cos(M_PI*i/50);
	rotate_y = sin(M_PI*i/50);

	for(uint8_t i = 0; i < 4; i++) {
	  N_X[i] = ROTATED_X(S_X[i], S_Y[i], rotate_x, rotate_y);
	  N_Y[i] = ROTATED_Y(S_X[i], S_Y[i], rotate_x, rotate_y);
	}


	for(uint8_t i = 0; i < 4; i++) {
	  lcd_drawLine(floor(center_x + N_X[i % 4]),
				   floor(center_y + N_Y[i % 4]),
				   floor(center_x + N_X[(i + 1) % 4]),
				   floor(center_y + N_Y[(i + 1) % 4]),
				   WHITE);
	}

	lcd_display();
	_delay_ms(10);
  }
}


static inline void point_matrix_mul(float out[3], float matrix[9], float point[3]) {
  float accum;
  for(uint8_t y = 0; y < 3; y++) {
	accum = 0;
	for(uint8_t x = 0; x < 3; x++) {
	  accum += point[x]*matrix[3*y + x];
	}

	out[y] = accum;
  }
}

void cube_disp(void) {
  /* float points[8][3] = { */
  /*	{-1, -1, -1}, */
  /*	{ 1, -1, -1}, */
  /*	{ 1,  1, -1}, */
  /*	{-1,  1, -1}, */
  /*	{-1, -1,  1}, */
  /*	{ 1, -1,  1}, */
  /*	{ 1,  1,  1}, */
  /*	{-1,  1,  1}, */
  /* }; */

  float ppoints[8][2];

  float s, c, div, h, j, k;

  for(uint16_t i = 0; i < 10000; i++) {
	lcd_clear_buffer();

	if((key_scan16(&keypad) != 0) && i > 10) {
	  return;
	}

	c = cos(M_PI*i/50);
	s = sin(M_PI*i/50);

	/* matrix[0] = c*c; */
	/* matrix[1] = c*s*s - s*c; */
	/* matrix[2] = c*s*c + s*s; */

	/* matrix[3] = s*c; */
	/* matrix[4] = s*s*s + c*c; */
	/* matrix[5] = s*s*c - c*s; */

	/* matrix[6] = -s; */
	/* matrix[7] = c*s; */
	/* matrix[8] = c*c; */

	for(uint8_t w = 0; w < 8; w++) {
	  // Hand "Optimization" to get rid of memory problems
	  h = (w % 4 == 0 ? -1 : 1)*(w % 4 == 3 ? -1 : 1);
	  j = (w % 4 < 2 ? -1 : 1);
	  k = (w < 4 ? -1 : 1);

	  div = 2.5 - s*h + c*s*j + c*c*k;
	  ppoints[w][0] = (c*c*h + (c*s*s - s*c)*j + (c*s*c + s*s)*k)/div;
	  ppoints[w][1] = (s*c*h + (s*s*s + c*c)*j + (s*s*c - c*s)*k)/div;


	  ppoints[w][0] = DISPLAY_WIDTH/2 + 32*ppoints[w][0];
	  ppoints[w][1] = DISPLAY_HEIGHT/2 + 32*ppoints[w][1];
	}

	for(uint8_t j = 0; j < 4; j++) {
	  lcd_drawLine((uint8_t)ppoints[j][0],
				   (uint8_t)ppoints[j][1],
				   (uint8_t)ppoints[(j + 1) % 4][0],
				   (uint8_t)ppoints[(j + 1) % 4][1],
				   WHITE);

	  lcd_drawLine((uint8_t)ppoints[j + 4][0],
				   (uint8_t)ppoints[j + 4][1],
				   (uint8_t)ppoints[(j + 1) % 4 + 4][0],
				   (uint8_t)ppoints[(j + 1) % 4 + 4][1],
				   WHITE);

	  lcd_drawLine((uint8_t)ppoints[j][0],
				   (uint8_t)ppoints[j][1],
				   (uint8_t)ppoints[j + 4][0],
				   (uint8_t)ppoints[j + 4][1],
				   WHITE);
	}

	lcd_display();
	_delay_ms(10);
  }
}

static menu_t SUB_MENU = {
  MenuType,
  "sub menu",
  0,
  0,
  11,
  {
	(menu_item_t*)&(menu_program_t){MenuProgramType, "Snake", &snake},
	(menu_item_t*)&(menu_program_t){MenuProgramType, "Square", &square_disp},
	(menu_item_t*)&(menu_program_t){MenuProgramType, "Cube", &cube_disp},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test1"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test2"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test3"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test4"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test5"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test6"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test7"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Test8"},
  }
};

static menu_t GAMES_MENU = {
  MenuType,
  "Games",
  0,
  0,
  9,
  {
	(menu_item_t*)&(menu_program_t){MenuProgramType, "Snake", &snake},
	(menu_item_t*)&(menu_program_t){MenuProgramType, "Hello World", &hello_world},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game2"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game3"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game4"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game5"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game6"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game7"},
	(menu_item_t*)&(menu_item_t){MenuItemType, "Game8"},
  }
};

static menu_t DEFAULT_MENU = {
  MenuType,
  "Default",
  0,
  0,
  2,
  {
	(menu_item_t*)&GAMES_MENU,
	(menu_item_t*)&SUB_MENU,
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
	lcd_puts(menu->items[i]->name);
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
  keypad = make_keypad(COLS, COLS_NO, ROWS, ROWS_NO);
  init_keypad(&keypad);

  lcd_init(LCD_DISP_ON);

  draw_menu(SELECTED_MENU);
  lcd_display();

  bool changed;

  while(true) {
	changed = true;

	current_mask = key_scan16(&keypad);
	char pressed = key_get_first_char_diff(&keypad, cmap, current_mask, last_mask);
	last_mask = current_mask;

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
	  menu_item_t* val = SELECTED_MENU->items[SELECTED_MENU->selected];

	  switch(val->type) {
	  case MenuType:
		MENU_STACK = &(menu_stack_t){MENU_STACK, SELECTED_MENU};
		SELECTED_MENU = (menu_t*)val;

		draw_menu(SELECTED_MENU);
		break;

	  case MenuProgramType:
		((menu_program_t*)val)->program_f();

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

	_delay_ms(10);
  }
}
