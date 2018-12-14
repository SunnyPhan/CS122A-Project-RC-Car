#include <avr/io.h>
#include <stdio.h>		/* Include std. library file */
#include <util/delay.h>		/* Include Delay header file */
#include <avr/interrupt.h>


int main(void)
{
	
	DDRB = 0xFF;
// 	PORTB |= (1 << PORTB2);	
	DDRC = 0xFF; PORTC = 5;
    //OCR0A = 100;
    // set PWM for 50% duty cycle


    TCCR0A |= (1 << COM0A1);		//0A makes it OC0A....?
    // set none-inverting mode

    TCCR0A |= (1 << WGM01) | (1 << WGM00);	//0A makes it OC0A....?
    // set fast PWM Mode

    TCCR0B |= (1 << CS01);
    // set prescaler to 8 and starts PWM
	
		
	while (1)
	{		
		OCR0A = 200;
	}
}