#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>

// PINOUT https://www.teachmemicro.com/wp-content/uploads/2019/06/Arduino-Nano-pinout-3.jpg

#include "lcd.h"
#include "keypad.h"

#define COLS_NO 3
#define ROWS_NO 4

int main(void) {
  out_pin_t COLS[COLS_NO] = {
	{&DDRB, &PORTB, DDB1}, // D9 1
	{&DDRD, &PORTD, DDD7}, // D7 3
	{&DDRD, &PORTD, DDD5}  // D5 5
  };

  in_pin_t ROWS[ROWS_NO] = {
	{&DDRB, &PORTB, &PINB, DDB0}, // D8 2
	{&DDRD, &PORTD, &PIND, DDD6}, // D6 4
	{&DDRD, &PORTD, &PIND, DDD4}, // D4 6
	{&DDRD, &PORTD, &PIND, DDD3}  // D3 7
  };

  keypad_t keypad = make_keypad(COLS, COLS_NO, ROWS, ROWS_NO);
  init_keypad(&keypad);

  lcd_init(LCD_DISP_ON);

  while(true) {
	_delay_ms(100);
	lcd_gotoxy(0, 0);

	if(key_pressed(&keypad, 0, 0)) {
	  lcd_puts("ON ");
	} else {
	  lcd_puts("OFF");
	}
  }


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
