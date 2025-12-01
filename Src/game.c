#include "game.h"
#include "gpio.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h>
#include "display.h"

#define SHIFT_TIME 200
#define TOTAL_GAME_TIME 10000 //total time for the game to run
static int MOLE_TIME = 250; //amount of time to wait in between potential mole generations
#define MOLE_CHANCE 8 //8 means 1/8 chance of appearing
static MOLE_TIMEOUT = 0; //amount of time to want before the mole disappears
#define MAX_MOLES 6 //max number of moles at a time

static PONG_TIME = 250;

static int mole_exists = 0; //1 if mole exists, 0 otherwise
static int mole_generate_time[8]; //when the mole was generated;
static int mole_location;
static uint8_t moles; //where there are moles on the board
static Time_t game_time = 0;
static Time_t select_press_time = 0;
static Time_t total_game_time;
static Time_t timeShift;
static int pos = 0;
static int dir = 0;
static uint8_t score = 0;
static Time_t button_debouncing;
static int position = 0;
static int direction = 0;

static int time_game_changed = 0; //when the game is changed from MOLE to PONG for debouncing

static enum {TITLEMOLE, TITLEPONG, GAMEMOLE, GAMEPONG, ENDMOLE, ENDPONG} state;


static int moles_temp; //temp storage of moles, this will hold whatever.

void Init_Game (void) {
	GPIO_PortEnable(GPIOX);
	timeShift = TimeNow();
	printf("Running init of mole game\n");
	state = TITLEMOLE;
	DisplayPrint(ALARM, 1, "MOLEMODE");
}

void Task_Game (void) {

	UpdateDisplay();

	switch (state) {
			case TITLEMOLE:

				// check if first button is pressed to switch to pong
				if (GPIO_PortInput(GPIOX) & 1 << (0 + 8) && TimePassed(time_game_changed) > 1000) { //check if button is pressed
					state = TITLEPONG;
					DisplayPrint(ALARM, 1, "PONGMODE");
					time_game_changed = TimeNow();
				}

				MOLE_TIMEOUT = MOLE_TIME*4;
				if (GPIO_PortInput(GPIOX) & 1 << (3 + 8) && TimePassed(select_press_time) > 1000) { //check if select is pressed
					select_press_time = TimeNow(); //so button can only be pressed once every 50ms or so
					switch (MOLE_TIME) {
						case 250:
							MOLE_TIME = 1000;
							printf("Selected 1000\n");
							break;
						case 500:
							MOLE_TIME = 250;
							printf("Selected 250\n");
							break;
						case 1000:
							MOLE_TIME = 500;
							printf("Selected 500\n");
							break;
					}
				} else if (GPIO_PortInput(GPIOX) & 1 << (4 + 8)) { //check if start is pressed
					GPIO_PortOutput(GPIOX, 0x00); //clear board
					mole_exists = 0;
					pos = 0;
					dir = 0;
					score = 0;
					state = GAMEMOLE;
					total_game_time = TimeNow();
					printf("Started Game\n");
				}

				//make lights turn on
				if (TimePassed(game_time) >= MOLE_TIME){ //only runs every MOLE_TIME ms
					if (1) {
						moles = 0x00;
						mole_location = rand() % 8;
						moles |= 1 << mole_location;
						GPIO_PortOutput(GPIOX, moles); // GENERATE the mole
					}
					game_time = TimeNow();
				}
				break;
			case TITLEPONG:
				// check if first button is pressed to switch to mole
				if (GPIO_PortInput(GPIOX) & 1 << (0 + 8) && TimePassed(time_game_changed) > 1000) { //check if button is pressed
					state = TITLEMOLE;
					DisplayPrint(ALARM, 1, "MOLEMODE");
					time_game_changed = TimeNow();
				}


				if (TimePassed(game_time) > PONG_TIME) {
					if (position == 7 || position == 0)
						direction ^=1;
					position += direction ? +1 : -1;
					GPIO_PortOutput(GPIOX, 1 << position);
					game_time = TimeNow();
				}

				if (GPIO_PortInput(GPIOX) & 1 << (3 + 8) && TimePassed(select_press_time) > 1000) { //check if select is pressed
					select_press_time = TimeNow(); //so button can only be pressed once every 50ms or so
					switch (PONG_TIME) {
						case 250:
							PONG_TIME = 100;
							printf("Selected 1000\n");
							break;
						case 500:
							PONG_TIME = 250;
							printf("Selected 250\n");
							break;
						case 100:
							PONG_TIME = 500;
							printf("Selected 500\n");
							break;
					}
				} else if (GPIO_PortInput(GPIOX) & 1 << (4 + 8)) { //check if start is pressed
					GPIO_PortOutput(GPIOX, 0x00); //clear board
					position = 1;
					direction = 1;
					score = 0;
					state = GAMEPONG;
					total_game_time = TimeNow();
					printf("Started Game\n");
				}

				break;
			case GAMEPONG:

				if (TimePassed(game_time) > PONG_TIME) {
					if (position == 7 || position == 0)
						state = ENDPONG;
					if (position == 1 || position == 2)
						if (GPIO_PortInput(GPIOX) & 1 << (1 + 8) || GPIO_PortInput(GPIOX) & 1 << (2 + 8) || GPIO_PortInput(GPIOX) & 1 << (0 + 8))
							direction = 1;
					if (position == 5 || position == 6)
						if (GPIO_PortInput(GPIOX) & 1 << (5 + 8) || GPIO_PortInput(GPIOX) & 1 << (6 + 8) || GPIO_PortInput(GPIOX) & 1 << (7 + 8))
							direction = 0;
					position += direction ? +1 : -1;
					GPIO_PortOutput(GPIOX, 1 << position);
					game_time = TimeNow();
				}

				break;
			case GAMEMOLE:
				if (TimePassed(total_game_time) < TOTAL_GAME_TIME) {
					//find number of moles active
					mole_exists = 0;
					moles_temp = moles;
					while (moles_temp) {
						mole_exists += moles_temp & 1;
						moles_temp >>= 1;
					}

					//maybe generate a mole
					if (mole_exists < MAX_MOLES) { //maybe generate mole
						if (TimePassed(game_time) >= MOLE_TIME){ //only runs every MOLE_TIME ms
							int randVal = rand() % MOLE_CHANCE;
							if (randVal) { //to randomize moles appearing
								mole_location = rand() % 8;
								moles |= 1 << mole_location;
								GPIO_PortOutput(GPIOX, moles); // GENERATE the mole
								mole_exists += 1;
								mole_generate_time[mole_location] = TimeNow();
							}
							game_time = TimeNow();
						}
					}

					//check for each mole if either it needs to be despawned or killed due to wacking
					for (int i = 0; i <= 7; i++) {
						int current_mole_exists = (moles & 1 << (i)) >> i;
						if (TimePassed(mole_generate_time[i]) > MOLE_TIMEOUT && current_mole_exists) { //despawn the mole
							if (score > 0)
								score--;
							printf("deapswning mole\n");
							mole_exists -= 1;
							moles &= ~(1 << i); //DEWPAWN the mole
							GPIO_PortOutput(GPIOX, moles);
						}
						if ((GPIO_PortInput(GPIOX) & 1 << (i + 8)) && (TimePassed(button_debouncing) > 200)) { //check if button beside LED is pressed
							printf("button killing mole\n");
							button_debouncing = TimeNow();
							moles &= ~(1 << i);
							GPIO_PortOutput(GPIOX, moles); // KILL the mole
							mole_exists -= 1;
							if (current_mole_exists)
								score++;
							else
								if (score > 0)
									score--;
						}
					}
				}
				else {
					state = ENDMOLE;
				}

				break;
			case ENDMOLE:
				//output score
				GPIO_PortOutput(GPIOX, score);
				DisplayPrint(ALARM, 1, "Game Complete");

				//check if start button is pressed
					if (GPIO_PortInput(GPIOX) & 1 << (3 + 8)) { //check if button is pressed
						state = TITLEMOLE;
					}

				break;
			case ENDPONG:
				DisplayPrint(ALARM, 1, "Game Complete");
				//check if start button is pressed
				if (GPIO_PortInput(GPIOX) & 1 << (3 + 8)) { //check if button is pressed
					state = TITLEPONG;
					position = 0;
					direction = 0;
				}
				break;

	}

}
