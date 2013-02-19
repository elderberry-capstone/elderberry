
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROCK	 1
#define PAPER	 4
#define SCISSORS 2
#define NUMTOWIN 5	/* The score needed to win the game */

#define true  1
#define false 0

/* Data Variables */
int device_click[2], score[2], click_val[2];
char game_buffer[2048];
int length;

/* Functions */
extern void fcf_callback_game(char *, int);
void resetTurn(void);
void resetAll(void);
void setValueClick(int, int);
char *returnValueName(int);
int checkValues();
void end_game(int, int);


void setValidClick(int val, int dev){
	click_val[dev] = val;
	device_click[dev] = true;
	length = sprintf(game_buffer, "Player %d has decided.\n", dev+1);
	fcf_callback_game(game_buffer, length);
}


void game_getDeviceClick(unsigned char *button, int dev){
	if(!device_click[dev]){
		switch((int)button[0]){
			case ROCK: 
				setValidClick(ROCK, dev);
				break;
			case PAPER:
				setValidClick(PAPER, dev);
				break;
			case SCISSORS:
				setValidClick(SCISSORS, dev);
				break;
		}
		//printf("Mouse %d: %d\n", dev, (int)button[0]);
		checkValues();
	}
}

char * returnValueName(int val){
	switch(val){
		case ROCK: 
			return "Rock";
		case PAPER:
			return "Paper";
		case SCISSORS:
			return "Scissors";
		default:
			return NULL;
	}
}

int checkValues(){
	if(device_click[0] && device_click[1]){
		if(click_val[0]==click_val[1]){
			length = sprintf(game_buffer, "\n* Tie! Both players chose %s\n\n", returnValueName(click_val[0]));
			fcf_callback_game(game_buffer, length);
			resetTurn();
		}
		else{
			int winner = 42;
			int loser = 42;
			if(click_val[0]==1 && click_val[1]==2){
				winner=0;
			}
			else if(click_val[0]==2 && click_val[1]==4){
				winner=0;
			}
			else if(click_val[0]==4 && click_val[1]==1){
				winner=0;
			}
			else{
				winner=1;
			}

			score[winner]++;

			if(winner){
				loser=0;
			}
			else{
				loser=1;
			}

			length = sprintf(game_buffer, "\n* Player %d wins matchup! %s beats %s\n\n\n", winner+1, returnValueName(click_val[winner]), returnValueName(click_val[loser]));
			fcf_callback_game(game_buffer, length);
			sleep(1);

			if(score[winner] >= NUMTOWIN){
				end_game(winner, loser);
			}
			else{
				resetTurn();
			}

			
		}
		
		return 0;
	}
	else{
		return -1;
	}
}


void end_game(int winner, int loser){
	length = sprintf(game_buffer, "\n\n\n\n********************************\n  Player %d wins game, %d to %d! \n********************************\n\n\n\n\n\n", winner+1, score[winner], score[loser]);
	fcf_callback_game(game_buffer, length);
	resetAll();
}


void resetTurn(){
	device_click[0] = false;
	device_click[1] = false;
	click_val[0] = 0;
	click_val[1] = 0;
	length = sprintf(game_buffer, "[Score: P1: %d | P2: %d] Ready... Go!\n", score[0], score[1]);
	fcf_callback_game(game_buffer, length);
}


void resetAll(){
	score[0] = 0;
	score[1] = 0;
	resetTurn();
}


void init_game(){
	system ( "clear" );
	length = sprintf(game_buffer, "\n======================================================\n   WELCOME TO ROCK, PAPER, SCISSORS!!1!one!\n======================================================\n\n");
	fcf_callback_game(game_buffer, length);
	resetAll();
}
