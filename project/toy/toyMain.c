#include <msp430.h>
#include "libTimer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


static enum {S1=0, S2=1, S3=2, S4=3, S5=4} state = S1;
static char sound_ind = 0;
static int sounds[][18] = {
	{0},
	{0, 10000, 6000, 0},
	{0, 8000, 4000, 0},
	{0, 4000, 8000, 0},
	{0, 8000, 8000, 8000, 4500, 8000, 8000, 4500, 6000, 8000, 8000, 8000, 4500, 8000, 8000, 4500, 6000, 0}
};
static int soundLen[] = {1, 4, 4, 4, 18}; 


void static stateAdvance() {
	switch (state) {
		case S1: // Play low two-tone sound byte, red on, green off
			red_on = 0; green_on = 0;
			break;
		case S2: // Play higher two-tone sound byte, red off, green on
			red_on = 1; green_on = 1;
			break;
		case S3: // Play highest two-tone sound byte, red on, green on
			red_on = 0; green_on = 1;
			break;
		case S4:
			red_on = 1; green_on = 0;
			break;
		case S5: // Play win sound, reset to first state
			red_on ^= 1; green_on ^= 1; // Toggle both LEDs
			break;
		default:
			red_on = 0; green_on = 0;
			state = S1;
	}
	led_changed = 1;
	led_update();
}


static void soundStateAdvance() {
	if (sound_ind >= 0 && sound_ind < soundLen[state])
		buzzer_set_period(sounds[state][sound_ind++]);
	else buzzer_set_period(0);
}


void __interrupt_vec(PORT1_VECTOR) Port_1() {   // Switch on P1 (S2)
	static char p1val, prev_p1val;
	if (P1IFG & SWITCHES) {	      // did a button cause this interrupt?
		P1IFG &= ~SWITCHES; // clear pending sw interrupts
		switch_interrupt_handler();
		p1val = ~switch_state_down; // Store switch press value
		if (p1val & (p1val ^ prev_p1val)) { // Only register press if switch was in off state
			state = (state > S5 || state < S1) ? S1 : state+1;
			sound_ind = 0; // Reset sound frequency state index
			stateAdvance();
		}
		prev_p1val = p1val;
	}
}


void __interrupt_vec(WDT_VECTOR) WDT() { // 250 interrupts/sec
	static char timerCount = 0;
	if (timerCount % 50 == 0) {
		if (state == S5) stateAdvance(); // Used for flashing state
		soundStateAdvance();
	}
	timerCount = (timerCount >= 250) ? 0 : timerCount+1;
}


int main() {
	configureClocks();
	buzzer_init();
	buzzer_set_period(0);
	led_init();
	switch_init();
	enableWDTInterrupts();
	or_sr(0x18);
}
