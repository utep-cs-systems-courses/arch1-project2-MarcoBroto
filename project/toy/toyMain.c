#include <msp430.h>
#include <stdio.h>
#include "libTimer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


static enum {S1=0, S2=1, S3=2, S4=3, S5=4} state = S1;
static char sound_ind = 0;
static int sounds[][10] = {
	{0},
	{10000, 6000, 0},
	{8000, 6000, 0},
	{4000, 8000, 0},
	{8000, 8000, 8000, 4500, 8000, 8000, 4500, 6000, 0}
};
static int soundLen[] = {1, 3, 3, 3, 9}; 


void static stateAdvance() {
	switch (state) {
		case S1: // Play low two-tone sound byte, red on, green off
			red_on = 0; green_on = 0;
			break;
		case S2: // Play higher two-tone sound byte, red off, green on
			red_on = 1; green_on = 0;
			break;
		case S3: // Play highest two-tone sound byte, red on, green on
			red_on = 0; green_on = 1;
			break;
		case S4:
			red_on = 1; green_on = 1;
			break;
		case S5: // Play win sound, reset to first state
			if (red_on == green_on) red_on = 0; green_on = 1;
			red_on ^= 1; green_on ^= 1;
			break;
		default:
			red_on = 0; green_on = 0;
			state = S1;
	}
	led_changed = 1;
	led_update();
}


static void soundStateAdvance() {
	if (sound_ind < soundLen[state])
		buzzer_set_period(sounds[state][sound_ind++]);
}


void __interrupt_vec(PORT1_VECTOR) Port_1() {   // Switch on P1 (S2)
	static char p1val, prev_p1val;
	if (P1IFG & SWITCHES) {	      // did a button cause this interrupt?
		P1IFG &= ~SWITCHES;		      // clear pending sw interrupts
		//switch_interrupt_handler();	// single handler for all switches
		p1val = switch_update_interrupt_sense();
		if (p1val & (p1val ^ prev_p1val)) { // Only register press if switch was in off state
			state += 1;
			sound_ind = 0;
			switch_state_changed = 0;
			stateAdvance();
		}
		if (state > S5 || state < S1) {
			state = S1;
			stateAdvance();
		}
		prev_p1val = p1val;
	}
}


void __interrupt_vec(WDT_VECTOR) WDT() { // 250 interrupts/sec
	static char timerCount = 0;
	switch (timerCound) {
		case 60: // Every ~1/4 second
			soundStateAdvance();
			if (state == 5) stateAdvance();
			break;
	}
	/*if (timerCount % 60 == 0) {
		soundStateAdvance();
		if (state == 5) stateAdvance();
	}*/
	timerCount = (timerCount >= 250) ? 0 : timerCount++;
}


int main() {
	configureClocks();
	buzzer_init();
	buzzer_set_period(0);
	led_init();
	switch_init();
	// enableWDTInterrupts();
	or_sr(0x18);
	return 0;
}
