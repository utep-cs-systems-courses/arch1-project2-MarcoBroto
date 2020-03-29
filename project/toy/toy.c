#include <msp430.h>
#include "libTimer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


static enum {S1=0, S2=1, S3=2, S4=3} state = S1;
static char sound_ind = 0;
static int sounds[][10] = {
	{10000, 6000, 0},
	{8000, 6000, 0},
	{4000, 8000, 0},
	{8000, 8000, 8000, 4000, 8000, 8000, 4000, 6000, 0}
};


void static stateAdvance() {
	switch (state) {
		case S1: // Play low two-tone sound byte, red on, green off
			red_on = 0;
			led_changed = 0;
			state = S2;
			break;
		case S2: // Play higher two-tone sound byte, red off, green on
			red_on = 1; green_on = 0;
			led_changed = 1;
			state = S3;
			break;
		case S3: // Play highest two-tone sound byte, red on, green on
			red_on = 0; green_on = 1;
			led_changed = 1;
			state = S4;
			break;
		case S4:
			red_on = 1; green_on = 1;
			led_changed = 1;
			break;
		default: // Play win sound, reset to first state
			red_on = 0; green_on = led_changed = 1;
			state = S1;
			break;
	}
	led_update();
}


static void soundStateAdvance() { buzzer_set_period(sounds[state][sound_ind++]); }


void __interrupt_vec(PORT1_VECTOR) Port_1() {   /* Switch on P1 (S2) */
	if (P1IFG & SWITCHES) {	      /* did a button cause this interrupt? */
		P1IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
		switch_interrupt_handler();	/* single handler for all switches */
		if (switch_state_down && switch_state_changed) {
			stateAdvance();
			switch_state_changed = 0;
		}
	}
}


void __interrupt_vec(WDT_VECTOR) WDT() { // 250 interrupts/sec
	static char blink_count = 0;
	if (++blink_count == 125) { // Every 0.5 second
		// soundStateAdvance();
		buzzer_set_period(sounds[state][sound_ind++]);
		blink_count = 0;
	} else if (blink_count == 30 && state == S4) {
		green_on ^= 1; red_on ^= 1;
	}
}


int main() {
	buzzer_init();
	buzzer_set_period(0);
	led_init();
	switch_init();
	enableWDTInterrupts();
	or_sr(0x18);
	return 0;
}
