#include <msp430.h>
#include "timer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


void stateAdvance() {
	static enum {A=0, B=1, C=2, D=3} state = A;
	static char sound_ind = 0;
	static int sounds[10];

	switch (state) {
		case A: // Play low two-tone sound byte, red on, green off
			red_on = 1;
			sounds = {10000, 6000, 0};
			state = B;
			break;
		case B: // Play higher two-tone sound byte, red off, green on
			red_on = 0; green_on = 1;
			sounds = {8000, 6000, 0};
			state = C;
			break;
		case C: // Play highest two-tone sound byte, red on, green on
			red_on = 1;
			sounds = {4000, 8000, 0};
			state = D;
			break;
		default: // Play win sound, reset to first state
			sounds = {8000, 8000, 8000, 4000, 8000, 8000, 4000, 6000, 8000, 0};
			state = A;
			break;
	}
	led_update();
}


void soundStateAdvance() {

}


void __interrupt_vec(PORT1_VECTOR) Port_1() {   /* Switch on P1 (S2) */
	if (P1IFG & SWITCHES) {	      /* did a button cause this interrupt? */
		P1IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
		switch_interrupt_handler();	/* single handler for all switches */
		if (switch_state_down) stateAdvance();
	}
}


void __interrupt_vec(WDT_VECTOR) WDT() { // 250 interrupts/sec
	static char blink_count = 0;
	if (++blink_count == 125) { // Every 0.5 second
		buzzer_set_period(sounds[sound_ind++]);
		blink_count = 0;
	}
}


int main() {
	buzzer_init();
	buzzer_set_period(0);
	led_init();
	switch_init();
	or_sr(0x18);
	return 0;
}
