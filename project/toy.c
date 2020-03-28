#include <msp430.h>
#include "timer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


void stateAdvance() {
	static enum {A=0, B=1, C=2, D=3} state = A;
	static enum instate = B;
	switch (state) {
		case A: // Play low two-tone sound byte, red on, green off
			switch (instate) {
				case A:
					buzzer_set_period(8000);
					instate = B;
					break;
				case B:
					buzzer_set_period(6000);
					instate = D;
					break;
				case C:
					buzzer_set_period(0);
					instate = D;
					break;
				default: break;
			}
			red_on = 1;
			state = B;
			break;
		case B: // Play higher two-tone sound byte, red off, green on
			switch (instate) {
				case A:
					buzzer_set_period(6000);
					instate = B;
					break;
				case B:
					buzzer_set_period(4000);
					instate = D;
					break;
				case C:
					buzzer_set_period(0);
					instate = D;
					break;
				default: break;
			}
			red_on = 0; green_on = 1;
			state = C;
			break;
		case C: // Play highest two-tone sound byte, red on, green on
			switch (instate) {
				case A:
					buzzer_set_period(4000);
					instate = B;
					break;
				case B:
					buzzer_set_period(2000);
					instate = D;
					break;
				case C:
					buzzer_set_period(0);
					instate = D;
					break;
				default: break;
			}
			red_on = 1;
			state = D;
			break;
		default: // Play win sound, reset to first state
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
	if (++blink_count == 125) {
		soundStateAdvance();
		blink_count = 0;
	}
}


int main() {
	buzzer_init();
	led_init();
	switch_init();
	or_sr(0x18);
	return 0;
}
