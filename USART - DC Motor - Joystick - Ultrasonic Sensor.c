#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>		/* Include std. library file */
#include <util/delay.h>		/* Include Delay header file */
#include <usart_ATmega1284.h>
#define  Trigger_pin	PA0	/* Trigger pin */

unsigned char distanceValue;
unsigned char Received_value = 0x00;

enum Receive_States { Wait, Received} Receive_State;


void Receive_States()
{
	switch (Receive_State)
	{
		case Wait:
		if(USART_HasReceived(1))
		{
			Receive_State = Received;
		}
		else
		{
			Receive_State = Wait;
		}	
		break;
		
		case Received:
		Receive_State = Wait;		
		break;
		
		default:
		Receive_State = Wait;
		break;
	}
	
	switch (Receive_State)
	{
		case Wait:
		PORTC = Received_value;
		break;
		
		case Received:
		Received_value = USART_Receive(1);
		USART_Flush(1);
		//PORTC = Received_value;
		break;
		
		default:
		break;
	}
}

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
	
}
void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}


	void read(){

		if((Received_value & 0xD0) == 0xD0)						//if sendValue is XXX1 XXXX then set PORTD = 1; Backward Direction
		{
			if(distanceValue <= 4){
			OCR0A = 0;}
			else{
			OCR0A = 240;
			//  			PORTC &= (0 << PORTC1);		//Change PB1 = 1 PB2 = 0; Reverse Direction
			//  			PORTC |= (1 << PORTC2);		//C1C2 - LEFT C3C4 - RIGHT
			//    			PORTC &= (0 << PORTC3);		//01 BACKWARD	01 BACKWARD
			//    			PORTC |= (1 << PORTC4);
			PORTC = 10;					//PORTC = XXXX 1010		
			}
		}
		else if((Received_value & 0xE0) == 0xE0)					//Otherwise if sendValue is XXX0 XXXX PORTD = 0; Forward Direction (Default)
		{
			OCR0A = 240;
			//  			PORTC &= (0 << PORTC2);		//Change PB1 = 0 PB2 = 1; Forward Direction
			//  			PORTC |= (1 << PORTC1);		//C1C2 - LEFT C3C4 - RIGHT
			//  			PORTC &= (0 << PORTC4);		//10 FORWARD	10 FORWARD
			//  			PORTC |= (1 << PORTC3);
			PORTC = 5;					//PORTC = XXXX 0101
		}
		else if((Received_value & 0x08) == 0x08)						//if sendValue = XXXX 10XX, PORTC = 6; Left Direction
		{														// PORTC = 0110
			OCR0A = 240;										// C4C3 = 01 C2C1 = 10
			PORTC = 9;											// Forward		Backward

		}
		else if((Received_value & 0x04) == 0x04)						//if sendValue = XXXX 01XX, PORTC = 9; Right Direction
		{																// PORTC = 1001
			OCR0A = 240;												// C4C3 = 10 C2C1 = 01
			PORTC = 6;													//	Backward	FoRward

		}
		else if((Received_value & 0x00) == 0x00)						//if sendValue = XXXX 00XX, PORTD = 0; Straight	(Default) and stopped
		{																
			OCR0A = 0;
			PORTC = 5;
		}
		
	}



int main(void)
{
	long count;
	unsigned char distance;
	unsigned char distanceCm;
		

	DDRA = 0x01;		/* Make trigger pin as output */
	PORTD = 0xFF;		/* Turn on Pull-up */
	DDRB = 0xFF;
	PORTB |= (1 << PORTB2);		//Makes PB3 OC0A as PWM Pin
	DDRC = 0xFF; PORTC = 0x00; 
	
	TimerSet(5);
	TimerOn();
	Receive_State = Wait;
	
	initUSART(1);
	
	
    TCCR0A |= (1 << COM0A1);		//0A makes it OC0A....?
    // set none-inverting mode

    TCCR0A |= (1 << WGM01) | (1 << WGM00);	//0A makes it OC0A....?
    // set fast PWM Mode

    TCCR0B |= (1 << CS01);
    // set prescaler to 8 and starts PWM
	
// 	sei();			/* Enable global interrupt */
// 	TIMSK1 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
// 	TCCR1A = 0;		/* Set all bit to zero Normal operation */
	
	while (1)
	{
		Receive_States();
		read();		
		
	
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTA |= (1 << Trigger_pin);
		_delay_us(10);
		PORTA &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR1 = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR1 = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR1 & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR1 = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR1 = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR1 & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 8MHz Timer freq, sound speed =343 m/s */
		
		distance = (double)count / 466.47;
		distanceValue = (double)count / 250;		
			

		
		
		while (!TimerFlag){}  // Wait for BL's period
		TimerFlag = 0;        // Lower flag
	}
}
