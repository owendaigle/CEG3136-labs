// Calculator app
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "calc.h"
#include "display.h"
#include "touchpad.h"


static Press_t input; // input
static Entry_t guess; //guess for current iteration
static int score; // Count of operands received
static int number; //number generated that we have to guess.

static enum {MENU, PROMPT, ENTRY, END} state;

// Assembly subroutines to be called as functions
uint32_t Increment(uint32_t n);
uint32_t Compare(uint32_t n, uint32_t n2);


// Initialization
void Init_Bing (void) {
	DisplayEnable();
	DisplayColor(BINGAME, MAGENTA);
	TouchEnable();
}


// Runtime
void Task_Bing (void) {
	switch (state) {
	case MENU:
		// Display menu
		DisplayPrint(BINGAME, 0, "BIN SRCH GAME");
		DisplayPrint(BINGAME, 1, "PRS 1 TO STRT");
		if (input != 1)
			input = TouchInput(BINGAME);
		if (input == 1) {
			state = ENTRY;
			number = rand()%100; //generates random number between 0 and 100
			DisplayPrint(BINGAME, 0, "GUESS NUMBER");
		}
		break;
	case PROMPT: //checks if the guess is right or wrong
		if (Compare(guess, number) == 0) { //calls assembly subroutine to compare two numbers, if eq
			state = END;
			input = NONE;
			guess = 0;
		}
		else {
			score = Increment(score);
			if (Compare(guess, number) == 1) //if less than
				DisplayPrint(BINGAME, 0, "WRNG, GUESS LO");
			else //if greater than
				DisplayPrint(BINGAME, 0, "WRNG, GUESS HI");
			guess = 0;
			state = ENTRY;
		}
		break;
	case ENTRY: //enters the guess
		bool done = TouchEntry(BINGAME, &guess);
		DisplayPrint(BINGAME, 1, "%u", guess);
		if (done) {
			state = PROMPT;
		}
		break;
	case END: //end page for the game
		DisplayPrint(BINGAME, 0, "CRCT. SCORE: %u", score);
		if (input == NONE)
			input = TouchInput(BINGAME);
		else {
			state = MENU;
			input = NONE;
			score = 0;
		}
		break;
	}
}
