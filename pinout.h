#ifndef PINOUT_H
#define PINOUT_H

#include <avr/io.h>
#include <stdbool.h>

// DIGITAL DATA DIRECTION REGISTERS
static volatile uint8_t* const DDDRS[] = {
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRD,
  &DDRB,
  &DDRB,
  &DDRB,
  &DDRB,
  &DDRB,
  &DDRB
};

// DIGITAL PORTS
static volatile uint8_t* const DPORTS[] = {
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTD,
  &PORTB,
  &PORTB,
  &PORTB,
  &PORTB,
  &PORTB,
  &PORTB,
};

// DIGITAL PIN REGISTERS
static volatile uint8_t* const DPINRS[] = {
  &PIND,
  &PIND,
  &PIND,
  &PIND,
  &PIND,
  &PIND,
  &PIND,
  &PIND,
  &PINB,
  &PINB,
  &PINB,
  &PINB,
  &PINB,
  &PINB,
};

// DIGITAL PINS
static const volatile uint8_t DPINS[] = {
  DDD0,
  DDD1,
  DDD2,
  DDD3,
  DDD4,
  DDD5,
  DDD6,
  DDD7,
  DDB0,
  DDB1,
  DDD2,
  DDB3,
  DDB4,
  DDB5,
};

typedef uint8_t dpin_t;

static inline void pin_direction(dpin_t pin, bool output) {
  if(output) {
	*(DDDRS[pin]) |= _BV(DPINS[pin]);
  } else {
	*(DDDRS[pin]) &= ~_BV(DPINS[pin]);
  }
}

static inline void pin_pullup(dpin_t pin, bool enable) {
  if(enable) {
	*(DPORTS[pin]) |= _BV(DPINS[pin]);
  } else {
	*(DPORTS[pin]) &= ~_BV(DPINS[pin]);
  }
}

static inline bool pin_read(dpin_t pin) {
  return (*DPINRS[pin] & _BV(DPINS[pin])) == _BV(DPINS[pin]);
}

#endif
