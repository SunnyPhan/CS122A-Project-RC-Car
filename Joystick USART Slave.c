#include <avr/io.h>
#include <avr/interrupt.h>
#include <usart_ATmega1284.h>

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

	void read(){
		if((Received_value & 0x08) == 0x08)						//if sendValue = XXXX 10XX, PORTD = 1; Left Direction
		{
			PORTC = 1;
		}
		else if((Received_value & 0x04) == 0x04)						//if sendValue = XXXX 01XX, PORTD = 2; Right Direction
		{
			PORTC = 2;
		}
		else if((Received_value & 0x00) == 0x00)						//if sendValue = XXXX 00XX, PORTD = 0; Straight	(Default)
		{
			PORTC = 0;
		}
		
		
		if((Received_value & 0x10) == 0x10)						//if sendValue is XXX1 XXXX then set PORTD = 1; Backward Direction
		{
			PORTC = 1;
		}
		else if((Received_value & 0x00) == 0x00)					//Otherwise if sendValue is XXX0 XXXX PORTD = 0; Forward Direction (Default)
		{
			PORTC = 0;
		}
		
		
	}



int main(void)
{
	/* Replace with your application code */
	
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	TimerSet(5);
	TimerOn();
	Receive_State = Wait;
	
	initUSART(1);
	
	while (1)
	{
		Receive_States();
		read();
		
		while (!TimerFlag){}  // Wait for BL's period
		TimerFlag = 0;        // Lower flag
	}
}
