#include <msp430.h>
#include <stdio.h>
#include "libTimer.h"
#include "switches.h"
#include "led.h"
#include "buzzer.h"


static FILE *log;
static enum {S1=0, S2=1, S3=2, S4=3, S5=5} state = S1;
static char sound_ind = 0;
static int sounds[][10] = {
	{10000, 6000, 0},
	{8000, 6000, 0},
	{4000, 8000, 0},
	{8000, 8000, 8000, 4500, 8000, 8000, 4500, 6000, 1}
};
static int soundLen[] = {3, 3, 3, 9}; 


void static stateAdvance() {
	switch (state) {
		case S1: // Play low two-tone sound byte, red on, green off
		  red_on = 0; green_on = 0;
			led_changed = 1;
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
			state = S5;
			break;
		default: // Play win sound, reset to first state
		  red_on = 0; green_on = 1;
		  led_changed = 1;
			state = S1;
			break;
	}
	led_changed = 1;
	led_update();
}


static void soundStateAdvance() {
  while (sound_ind++ < soundLen[state])
    buzzer_set_period(sounds[state][sound_ind++]);
}


void __interrupt_vec(PORT1_VECTOR) Port_1() {   // Switch on P1 (S2)
  if (P1IFG & SWITCHES) {	      // did a button cause this interrupt?
    P1IFG &= ~SWITCHES;		      // clear pending sw interrupts
    switch_interrupt_handler();	// single handler for all switches
    fprintf(log, "Switch Interrupt:\nswitch_state_down=%d\nswitch_state_changed=%d\n\n");
		if (switch_state_down && switch_state_changed) {
			stateAdvance();
			switch_state_changed = 0;
		}
	}
}


void __interrupt_vec(WDT_VECTOR) WDT() { // 250 interrupts/sec
  static char blink_count = 0;
	if (++blink_count == 60) { // Every 0.5 second
		soundStateAdvance();
		if (blink_count == 30 && state == S5) {
		  green_on ^= 1; red_on ^= 1;
		}
	}
}


int main() {
  log = fopen("output.log", "a+");
	buzzer_init();
	buzzer_set_period(0);
	led_init();
	switch_init();
	enableWDTInterrupts();
	or_sr(0x18);
	return 0;
}
