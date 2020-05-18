/*	Author: sflei002
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}
//---------------------------------------------------
enum TL_States {TL_init, TL_l0, TL_l1, TL_l2} TL_state;
unsigned char TL_tmp = 0x00;
void Tick_Threeleds(){
	switch(TL_state){
		case TL_init:
			TL_state = TL_l0;
			break;
		case TL_l0:
			TL_state = TL_l1;
			break;
		case TL_l1:
			TL_state = TL_l2;
			break;
		case TL_l2:
			TL_state = TL_l0;
			break;
		default:
			TL_state = TL_init;
			break;
		
	}

	switch(TL_state){
		case TL_init:
			TL_state = TL_l0;
			break;
		case TL_l0:
			TL_tmp = 0x01;
			break;
		case TL_l1:
			TL_tmp = 0x02;
			break;
		case TL_l2:
			TL_tmp = 0x04;
			break;
		default:
			break;
	}
}
//--------------------------------------------------
enum BL_States {BL_init, BL_off, BL_on} BL_state;
unsigned char BL_tmp = 0x00;
void Tick_Blinkingled(){
        switch(BL_state){
                case BL_init:
                        TL_state = BL_off;
                        break;
                case BL_off:
                        BL_state = BL_on;
                        break;
                case BL_on:
                        BL_state = BL_off;
                        break;
                default:
                        BL_state = BL_init;
                        break;

        }

        switch(BL_state){
                case BL_init:
			BL_state = BL_off;
                        break;
                case BL_off:
			BL_tmp = 0x00;
                        break;
                case BL_on:
			BL_tmp = 0x08;
                        break;
                default:
                        break;
        }
}
//--------------------------------------------------
enum CL_States {CL_init, CL_1} CL_state;
void Tick_Combineleds(){
        switch(CL_state){
                case CL_init:
                        CL_state = CL_1;
                        break;
                case CL_1:
                        break;
                default:
                        CL_state = CL_init;
                        break;

        }

        switch(CL_state){
                case CL_init:
			CL_state = CL_1;
                        break;
                case CL_1:
                        PORTB = (TL_tmp | BL_tmp);
                        break;
                default:
                        break;
        }
}


//--------------------------------------------------
int main(void) {
    unsigned long BL_elapsedTime = 100;
    unsigned long TL_elapsedTime = 100;
    const unsigned long BL_period = 100;
    const unsigned long TL_period = 100;
    const unsigned long timerPeriod = 1;

    DDRB = 0xFF;
    PORTB = 0x00;
    TimerSet(timerPeriod);
    TimerOn();
    BL_state = -1;
    TL_state = -1;
    CL_state = -1;
    while (1) {
	if(TL_elapsedTime >= TL_period){
		Tick_Threeleds();
		TL_elapsedTime = 0;
	}
	if(BL_elapsedTime >= BL_period){
                Tick_Blinkingled();
                BL_elapsedTime = 0;
        }
	Tick_Combineleds();
	while(!TimerFlag) {}
	TimerFlag = 0;
	BL_elapsedTime += timerPeriod;
	TL_elapsedTime += timerPeriod;
    }
}
