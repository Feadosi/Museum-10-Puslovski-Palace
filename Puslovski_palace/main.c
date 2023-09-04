/*
 * Puslovski_palace.c
 *
 * Created: 8/9/2023 6:43:58 PM
 * Author : feodosiy
 */ 

/*---------Defines for includes----------*/
#define F_CPU 8000000UL
/*---------------------------------------*/

/*--------------Includes-----------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
/*---------------------------------------*/

/*---------------Defines-----------------*/
#define DEL 5882 //Delay for 8 circular LEDs
/*----------------*/
#define LEDsA_ddr DDRA
#define LEDsD_ddr DDRD
#define BUTTs_ddr DDRC
/*----------------*/
#define LEDsA      PORTA
#define LEDsD      PORTD
#define BUTT_WRITE PORTC
/*----------------*/
#define butt 7
#define BUTT_READ PINC
/*----------------*/
#define led1  PORTD0
#define led2  PORTD1
#define led3  PORTD2
#define led4  PORTD3
#define led5  PORTD4
#define led6  PORTD5
#define roof  PORTD6
#define flor  PORTD7
#define house PORTA7
/*---------------------------------------*/


/*--------------Variables----------------*/
/*PWM duty cycle in the program*/
uint8_t DCled1 = 255, DCled2 = 255, DCled3 = 255, \
		DCled4 = 255, DCled5 = 255, DCled6 = 255, \
		DChouse = 255, DCflor = 0, DCroof = 0;

/*PWM duty cycle buffers*/      
uint8_t DCled1_b, DCled2_b, DCled3_b, DCled4_b, \
		DCled5_b, DCled6_b, DChouse_b, DCflor_b,\
		DCroof_b;                        
uint8_t count, n;      //interrupt handler call counter
uint8_t temp = 1;      //to change LEDs
/*---------------------------------------*/

/*--------------Functions----------------*/
/*------------function for setting the backlight duty cycle-------------*/
void dutyCycle() {
	while(1){
		if (temp == 1) {
			if (n < 255) {
				if(DCflor < 150) DCflor += 1; //smoothly extinguishes the floor by 60% and
				if(DCroof < 130) DCroof+=1;   // the ceiling by 70%
				n++;
				_delay_us(DEL);
			}
			else temp = 2; //go to the next LED
		}
		if (temp == 2) {
			if (DCled1 > 0) {
				DCled1 -= 1;
				if(DCflor < 255) DCflor += 1;
				if(DCroof < 170) DCroof += 2; if((DCroof >= 170) && (DCroof < 255)) DCroof += 1; _delay_us(DEL);
			}   //we extinguish the floor and ceiling smoothly by lighting the first lightode. ->
			//P.S. Two if for the ceiling are used to extinguish the ceiling and the floor at the same time
			else temp = 3;
		}
		if (temp == 3) {
			if (DCled1 < 255) {
				DCled1 += 1;
				if(DCled1 > 100) DCled2 -= 1;  //then gradual lighting and extinguishing of 8 LEDs in overlapping
				_delay_us(DEL);
			}
			else temp = 4;
		}
		if (temp == 4) {
			if (DCled2 > 0) {
				DCled2 -= 1;
				_delay_us(DEL);
			}
			else temp = 5;
		}
		if (temp == 5) {
			if (DCled2 < 255) {
				DCled2 += 1;
				if(DCled2 > 100) DCled3 -= 1;
				_delay_us(DEL);
			}
			else temp = 6;
		}
		if (temp == 6) {
			if (DCled3 >   0) {
				DCled3 -= 1;
				_delay_us(DEL);
			}
			else temp = 7;
		}
		if (temp == 7) {
			if (DCled3 < 255) {
				DCled3 += 1;
				if(DCled3 > 100) DChouse -= 1;
				_delay_us(DEL);
			}
			else temp = 8;
		}
		if (temp == 8) {
			if (DChouse > 0) {
				DChouse -= 1;
				_delay_us(DEL);
			}
			else {temp = 9; _delay_ms(4000);}
		}
		if (temp == 9) {
			if (DChouse < 255) {
				DChouse += 1;
				if(DChouse > 100) DCled4 -=1;
				_delay_us(DEL);
			}
			else temp = 10;
		}
		if (temp == 10) {
			if (DCled4 > 0) {
				DCled4 -= 1;
				_delay_us(DEL);
			}
			else temp = 11;
		}
		if (temp == 11) {
			if (DCled4 < 255) {
				DCled4 += 1;
				if(DCled4 > 100) DCled5 -= 1;
				_delay_us(DEL);
			}
			else temp = 12;
		}
		if (temp == 12) {
			if (DCled5 > 0) {
				DCled5 -= 1;
				_delay_us(DEL);
			}
			else temp = 13;
		}
		if (temp == 13) {
			if (DCled5 < 255) {
				DCled5 += 1;
				if(DCled5 > 100) DCled6 -= 1;
				_delay_us(DEL);
			}
			else temp = 14;
		}
		if (temp == 14) {
			if (DCled6 > 0) {
				DCled6 -= 1;
				_delay_us(DEL);
			}
			else temp = 15;
		}
		if (temp == 15) {
			if (DCled6 < 255) {
				DCled6 += 1;
				if((DCroof > 130) && (DCled6 > 125)) DCroof -=1; //smoothly light the ceiling at 70%
				_delay_us(DEL);
			}
			else temp = 16;
		}
		if (temp == 16) {
			if (DCflor > 0) {
				DCflor -= 1;
				if(DCroof > 0) DCroof -= 1; //smoothly light the floor and ceiling at 100%
				_delay_us(DEL);
			}
			else {temp = 1; n = 0; return;} //return the parameters to the initial ones and finish the function
		}
	}
}
/*----------------------------------------------------------------------*/


/*------------------------initialize ports------------------------------*/
void initial_ports() {
	LEDsA_ddr = 0xFF;		//initialize port A
	LEDsA = 0x00;

	LEDsD_ddr = 0xFF;		//initialize port D
	LEDsD = 0x00;
	LEDsD |= (1<<roof) | (1<<flor);	 //because the roof and floor are always lit by default

	BUTTs_ddr &= ~(1<<butt);		//initialize port C on the button
	BUTT_WRITE |= (1<<butt);	//pull up buttons to 5V
}
/*----------------------------------------------------------------------*/


/*--------------------overflow interrupt handler------------------------*/
ISR (TIMER0_OVF_vect) {
	count++;
	if (count == 0) {  //if the counter overflowed and took the value 0
		/*save the values of duty cycle to the buffer of all 8 LEDs, layout building, ceiling and floor*/
		DCled1_b  = DCled1;
		DCled2_b  = DCled2;
		DCled3_b  = DCled3;
		DCled4_b  = DCled4;
		DCled5_b  = DCled5;
		DCled6_b  = DCled6;
		DCroof_b  = DCroof;
		DCflor_b  = DCflor;
		DChouse_b = DChouse;
		
		LEDsA=0b00000000;      //set the pins of ports A 
		LEDsD=0b00000000;      //and D to zeros           
	}
	
	/*when the set cycle rate is reached, output a logic 1 to the pin*/
	if ((DCled1_b  == count)  && (DCled1_b  != 255))  { LEDsD |= (1<<led1);}
	if ((DCled2_b  == count)  && (DCled2_b  != 255))  { LEDsD |= (1<<led2);}
	if ((DCled3_b  == count)  && (DCled3_b  != 255))  { LEDsD |= (1<<led3);}
	if ((DCled4_b  == count)  && (DCled4_b  != 255))  { LEDsD |= (1<<led4);}
	if ((DCled5_b  == count)  && (DCled5_b  != 255))  { LEDsD |= (1<<led5);}
	if ((DCled6_b  == count)  && (DCled6_b  != 255))  { LEDsD |= (1<<led6);}
	if ((DCroof_b  == count)  && (DCroof_b  != 255))  { LEDsD |= (1<<roof);}
	if ((DCflor_b  == count)  && (DCflor_b  != 255))  { LEDsD |= (1<<flor);}
	if ((DChouse_b == count)  && (DChouse_b != 255))  { LEDsA |= (1<<house);}
}

/*------------------------------Main------------------------------------*/
int main(void) {
	initial_ports();		//initialize the ports

	TIMSK &= ~(1 << TOIE0);	//enable timer overflow interrupt
	TCCR0 |= (1 << CS00);	//timer setting, prescaler off
	sei();					//enable global interrupts

	while (1) {
		if (!(BUTT_READ & (1 << butt))) {   //push the button for a logical zero
			TIMSK |= (1 << TOIE0);  //allow interruptions to overflow
			dutyCycle();	        //execute the cycle
			TIMSK &= ~(1 << TOIE0);	//prohibit overflow interrupts
		}
		_delay_ms(500);             //protection contact bounce
	}
}
/*----------------------------------------------------------------------*/


