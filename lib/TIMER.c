/* TIMER.c
 *
 * Copyright 2009-2013 Nixotic Design
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TIMER.h"

volatile uint8_t timer0_triggered = FALSE;
volatile uint8_t timer0_alpha = 0;
volatile uint16_t counter0 = 0;
volatile uint8_t timer1_triggered = FALSE;

void timer0_basic( uint8_t compare_value, uint8_t prescaler )
{
#ifdef ATMEGA128
  TIMSK |= (1 << OCIE0);
  TCCR0 |= (1 << WGM01); // Configure timer 0 for CTC mode
  OCR0 = compare_value; // Set the compare value
  TCCR0 |= prescaler; // Start timer
#else
#ifdef ATINY2313
  TIMSK |= (1 << OCIE0A); 
#else
  TIMSK0 |= (1 << OCIE0A); 
#endif
  TCCR0A |= (1 << WGM01); // Configure timer 0 for CTC mode
  OCR0A = compare_value; // Set the compare value
  TCCR0B = prescaler; // Start timer
#endif
  timer0_triggered = FALSE;
  timer0_alpha = 0;
}

void timer1_basic( void )
{
  // notes:
  // 8MHz clock. 16 bit normal operation.
  
  // WGM13:0 = 0 normal
  // COM1xn = 0 output compare disconnected
  // CS12:11:10 = 0b100 (clk-io / 256)
  
  TCCR1A = 0; 
  TCNT1H = 0; //Clear timer
  TCNT1L = 0; //Clear timer
#ifdef ATTINY2313
  TIMSK |= _BV(TOIE1);
#else
  TIMSK1 |= _BV(TOIE1);
#endif
  TCCR1B = _BV(CS12); //Set clock source and  start timer
}

void timer1_tic( void )
{
  // notes:
  // 8MHz clock. 16 bit normal operation.
  
  // WGM13:0 = 0 normal
  // COM1xn = 0 output compare disconnected
  // CS12:11:10 = 0b100 (clk-io / 256)
	
  TCCR1A = 0; 
  TCCR1B = _BV(WGM12); //CTC (only works on OCR1A!) 
  TCNT1H = 0; //Clear timer
  TCNT1L = 0; //Clear timer
  OCR1A = 62500; //8E6/256/31250 = 1
  OCR1B = 62500; //8E6/256/31250 = 1
#ifdef ATTINY2313
  TIMSK |= _BV(OCIE1A)| _BV(OCIE1B);
#else
  TIMSK1 |= _BV(OCIE1A)| _BV(OCIE1B);
#endif
  TCCR1B |= _BV(CS12); //Set clock source and  start timer
}


ISR( TIMER0_COMPA_vect )
{
  timer0_triggered = TRUE;
  timer0_alpha++;
}

ISR( TIMER1_OVF_vect )
{
  timer1_triggered = TRUE;
}

ISR( TIMER1_COMPA_vect )
{
  timer1_triggered = TRUE;
}

EMPTY_INTERRUPT( TIMER1_COMPB_vect )
