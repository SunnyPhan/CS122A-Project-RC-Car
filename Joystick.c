/*
 * JOYSTICK.c
 *
 * Created: 11/27/18 1:47:19 PM
 * Author : sunnyphan
 */ 

#include <avr/io.h>

	unsigned short joyX, joyY, joyPress; 
	unsigned char sendValue = 0x00;


	void convert(){
		
		Set_A2D_Pin(0x01);		//Set ADC to A0 pin to read L/R
		joyX = ADC; 
		sendValue = (sendValue & 0xF3);			// Set to XXXX 00XX
		
		if(joyX >= 600)
		{
			sendValue = (sendValue & 0xF3) | 0x08;			//if ADC A0 value is greater than 600 (left), set sendValue XXXX 10XX 
			PORTB = 1;
		} 
		else if(joyX < 424)
		{
			sendValue = (sendValue & 0xF3) | 0x04;			//if ADC A0 value is less than 424 (right), set sendValue XXXX 01XX 
			PORTB = 2;
		}
		else
		{
			sendValue = (sendValue & 0xF3);					//if ADC A0 value is between 424-600 (straight), set sendValue XXXX 00XX 
			PORTB = 0;
		}
			
		
		Set_A2D_Pin(0x02);		//Set ADC to A1 pin to read U/D
		joyY = ADC; 
		sendValue = (sendValue & 0xEF);		//Set to XXX0 XXXX
		
		if(joyY < 424)
		{
			sendValue = (sendValue & 0xEF) | 0x10;			//if ADC A1 value is less than 424(backward), set sendValue XXX1 XXXX; Backward Direction
		}
		else
		{
			sendValue = (sendValue & 0xEF);					//else set U/D to default U, which is XXX0 XXXX	 Default Forward
		}
// 		
// 		Set_A2D_Pin(0x03);									// Read ADC from A2 pin 
// 		joyPress = ADC;										// Set ADC as joystick press variable
// 		
// 		
// 		if(joyPress >= 20){									// if the joystick is press, the ADC value will be below 20
// 		PORTB = 1;											// if not pressed, will read a higher value. 
// 		} else {
// 			PORTC = 0;
// 		}


	}
	
	void read(){
		if((sendValue & 0x08) == 0x08)						//if sendValue = XXXX 10XX, PORTD = 1; Left Direction
		{
			PORTC = 1;
		}
		else if((sendValue & 0x04) == 0x04)						//if sendValue = XXXX 01XX, PORTD = 2; Right Direction
		{
			PORTC = 2;
		}
		else if((sendValue & 0x00) == 0x00)						//if sendValue = XXXX 00XX, PORTD = 0; Straight	(Default)
		{
			PORTC = 0;
		}
		
		
		if((sendValue & 0x10) == 0x10)						//if sendValue is XXX1 XXXX then set PORTD = 1; Backward Direction
		{
			PORTC = 1;
		}
		else if((sendValue & 0x00) == 0x00)					//Otherwise if sendValue is XXX0 XXXX PORTD = 0; Forward Direction (Default)
		{
			PORTC = 0;
		}
		
		
	}

	void A2D_init() {
		ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
		// ADEN: Enables analog-to-digital conversion
		// ADSC: Starts analog-to-digital conversion
		// ADATE: Enables auto-triggering, allowing for constant
		//	    analog to digital conversions.
	}
	
	void Set_A2D_Pin(unsigned char pinNum) {
		ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
		// Allow channel to stabilize
		static unsigned char i = 0;
		for ( i=0; i<15; i++ ) { asm("nop"); }
	}


int main(void)
{	
	DDRA = 0x00;   
	DDRC = 0xFF; PORTC = 0x00;
	A2D_init();

	
    /* Replace with your application code */
    while (1) 
    {
		unsigned short input = ADC;
		
		convert();
		read();
		
		
// 		Set_A2D_Pin(0x01);		//Set ADC to A0 pin to read L/R
// 		if(input >= 600){
// 		PORTC = 1;
// 		} else if(input < 424){
// 		PORTC = 2;
// 		} else{
// 			PORTC = 0;
// 		}


		
    }
}

