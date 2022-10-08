/*
 * pro_2.c
 *
 *  Created on: Sep 13, 2022
 *      Author: felob
 */
# include <avr/io.h>
# include <util/delay.h>
#include <avr/interrupt.h>

unsigned short G_count=0,G_sec=0,G_min=0,G_min2=0,G_hour=0,G_hour2=0;

ISR(INT0_vect)
{
	G_count=0;  //reset stop watch
	G_sec=0;
	G_min=0;
	G_min2=0;
	G_hour=0;
	G_hour2=0;
}
ISR(INT1_vect){
	TCCR1B&=~(1<<CS10) &~(1<<CS11) &~(1<<CS12); //stop stop watch count-up
}
ISR(INT2_vect){
	TCCR1B |=(1<<CS12) |(1<<CS10);  //resume stop watch count-up
}
ISR(TIMER1_COMPA_vect){
	if((G_hour==4)&&(G_hour2==2)){
		G_count=0;   //if it completes maximum time 24 hours it will reset automatically
		G_sec=0;
		G_min=0;
		G_min2=0;
		G_hour=0;
		G_hour2=0;
	}
	if(G_count==9){ //condition to increase tenth number every 10 secs
		G_count=0;
		if(G_sec==5){ //condition to increase 1 min every 60 second
			G_sec=0;
			if(G_min==9){
				G_min=0;
				if(G_min2==5){ //condition to inrease 1 hour every 60 mins
					G_min2=0;
					if(G_hour==9){
						G_hour=0;
						G_hour2++;
					}
					else{G_hour++;}
				}
				else{G_min2++;}
			}
			else{G_min++;}
		}
		else{G_sec++;}

	}
	else {G_count++;}
}

void config(void){
	TCNT1 = 0; //start value of timer
	OCR1A = 976; //end value before interrupt
	TIMSK |= (1<<OCIE1A);
	TCCR1A = (1<<FOC1A); //timer work at compare mode
	TCCR1B = (1<<WGM12) | (1<<CS12) |(1<<CS10);// FCPU/1024
}

void INT0_init(void){
	DDRD &= ~(1<<PD2);    //make PD2 INPUT PIN
	PORTD |= (1<<PD2);    //as it's pull down show in first 1 on pin
	GICR |= (1<<INT0);    //enable int0
	MCUCR |= (1<<ISC01);   //make interrupt raise flag at falling edge
	MCUCR &= ~(1<<ISC00);
}

void INT1_init(void){
	DDRD &= ~(1<<PD3); //make PD3 INPUT PIN
	GICR |= (1<<INT1); //enable int1
	MCUCR |= (1<<ISC10) | (1<<ISC11); //make interrupt raise flag at rising edge
}

void INT2_init(void){
	DDRB &= ~(1<<PB2);  //make PB2 INPUT PIN
	PORTB |= (1<<PB2);  //as it's pull down show in first 1 on pin
	GICR |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2); //make interrupt raise flag at falling edge
}


int main(void){
	SREG|=(1<<7);   //enable i-bit
	INT0_init();    //int0 initialization function
	INT1_init();    //int1 initialization function
	INT2_init();    //int2 initialization function
	config();

	DDRC|=0x0F; //make first 4 bits in port C input pins
	DDRA|=0x3F; //make first 7 bits in port A input pins
	while(1){
		PORTA|=(1<<PA0);   //enable every bit continously to display all bits at once
		PORTC=(PORTC&0xF0)|G_count; //show every number on a distinct 7 segment
		_delay_ms(4);
		PORTA&=~(1<<PA0);
		PORTA|=(1<<PA1);
		PORTC=(PORTC&0xF0)|G_sec;
		_delay_ms(4);
		PORTA&=~(1<<PA1);
		PORTA|=(1<<PA2);
		PORTC=(PORTC&0xF0)|G_min;
		_delay_ms(4);
		PORTA&=~(1<<PA2);
		PORTA|=(1<<PA3);
		PORTC=(PORTC&0xF0)|G_min2;
		_delay_ms(4);
		PORTA&=~(1<<PA3);
		PORTA|=(1<<PA4);
		PORTC=(PORTC&0xF0)|G_hour;
		_delay_ms(4);
		PORTA&=~(1<<PA4);
		PORTA|=(1<<PA5);
		PORTC=(PORTC&0xF0)|G_hour2;
		_delay_ms(4);
		PORTA&=~(1<<PA5);
	}
}


