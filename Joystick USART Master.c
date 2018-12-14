#include <avr/io.h>
#include <avr/interrupt.h>
#include <usart_ATmega1284.h>

	unsigned short joyX, joyY, joyPress; 
	unsigned char sendValue = 0x00;


	void convert(){
		
		Set_A2D_Pin(0x01);		//Set ADC to A0 pin to read L/R
		joyX = ADC; 
		sendValue = (sendValue & 0xF3);			// Set to XXXX 00XX
		
		if(joyX >= 600)
		{
			sendValue = (sendValue & 0xF3) | 0x08;			//if ADC A0 value is greater than 600 (left), set sendValue XXXX 10XX 
		} 
		else if(joyX < 424)
		{
			sendValue = (sendValue & 0xF3) | 0x04;			//if ADC A0 value is less than 424 (right), set sendValue XXXX 01XX 
		}
		else
		{
			sendValue = (sendValue & 0xF3);					//if ADC A0 value is between 424-600 (straight), set sendValue XXXX 00XX 
		}
			
		
		Set_A2D_Pin(0x02);		//Set ADC to A1 pin to read U/D
		joyY = ADC; 
		sendValue = (sendValue & 0xCF);		//Set to XX00 XXXX
		
		if(joyY >= 600)
		{
			sendValue = (sendValue & 0xEF) | 0xE0;			//if ADC A1 value is greater than 600(backward), set sendValue XX10 XXXX; Backward Direction
		}
		else if(joyY < 424)
		{
			sendValue = (sendValue & 0xEF) | 0xD0;			//if ADC A1 value is less than 424(forward), set sendValue XX01 XXXX; Forward Direction
		} else
		{
			sendValue = (sendValue & 0xEF);					//if ADC A1 value is between 424-600, set sendValue to XX00 XXXX; Stopped
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
		else if((sendValue & 0x10) == 0x10)						//if sendValue is XXX1 XXXX then set PORTD = 1; Backward Direction
		{
			PORTC = 3;
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
	initUSART(0);
	
    /* Replace with your application code */
    while (1) 
    {
		unsigned short input = ADC;
		
		convert();
		USART_Send(sendValue,0);
				
		read();
				
		
//  		Set_A2D_Pin(0x02);		//Set ADC to A0 pin to read L/R
//  		if(input >= 600){
//  		PORTC = 1;
//  		} else if(input < 424){
//  		PORTC = 2;
//  		} else{
//  			PORTC = 0;
//  		}


		
    }
}

