#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC


#define LED_GREEN PB5
int main(void)
{
    GPIO_mode_output(&DDRB, LED_GREEN);

    GPIO_write_low(&PORTB, LED_GREEN);
    
    TIM1_ovf_33ms();

    TIM1_ovf_enable();
    //global interrupt
    sei();
    
    //forever loop
    while(1)
    {

    }
    return 0;
}

ISR(TIMER1_OVF_vect)
{
  static int8_t n_ovfs = 0;

  n_ovfs++;
  if(n_ovfs >= 3)
  {
    PORTB = PORTB ^ (1<<LED_GREEN);
  }
  
}