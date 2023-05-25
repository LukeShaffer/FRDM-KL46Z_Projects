/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    ttt.c
 * @brief   Application entry point.
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "math.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"

// User defined header files
#include "slcd.h"
#include "touchsensor.h"

// global variables
int total_games;
int games_played;
int total_wins;

//for random numbers
#define PLAYER_TURN 0x0
#define AI_TURN 0x1
// tictactoe symbols
#define O_SYMBOL 'O'
#define X_SYMBOL 'X'
// Board size macros
#define SMALL_BOARD 3
#define MEDIUM_BOARD 7
#define LARGE_BOARD 16

// wait function macro
#define CYCLES_TO_MS 3000 //1ms

char last_size;

// tic tac toe game
unsigned char **board;
// number of empty squares
int empty_squares;
// size of the board
int board_size;

// variables for recognizing the touch
int last_touch;
int current_turn;

//the last state of the touch to prevent multiple messages
int current_touch;

/*
 * Function helps the user decide the dimensions of the board.
 */
int get_board_size(){
	int to_return;
	printf("What size board would you like to play?\n");
	scanf("%d", &to_return);
	return to_return;
}

/*
 * Wait function on the board. Waits in miliseconds
 */
void wait_ms(int ms_to_wait){
	unsigned int i;

	for (i = 0; i < ms_to_wait * CYCLES_TO_MS; i++){
		__asm__("");
	}
}


/*
 * This function uses the touch sensor to get user input for recognizing board size.
 */
int touch_range(){
    int min_touch = 60;
    int max_touch = 1100;
    int num_bins = 3;
    char board_size[7];
    char large[] = "LARGE";
    char medium[] = "MEDIUM";
    char small[] = "SMALL";
    int interval = (int) ((max_touch - min_touch) / num_bins);


    int to_return;

    //get the last registered value from the touch sensor
    last_touch = current_touch;

    int avg = 0;
    printf("What size board would you like to play?\n");
    printf("Select the size of board you want to play on the touchscreen: \n1) SMALL: 3x3\n2) MEDIUM: 7x7\n3) LARGE: 16x16\n");
    printf("***************************************************************************************\n");
    printf("*      KEEP TOUCHING ONE THESE AREAS FOR THE NEXT 5 SECONDS TO SELECT A SIZE          *\n");
    printf("*                     |                                     |                         *\n");
    printf("*   SMALL             |              MEDIUM                 |            LARGE        *\n");
    printf("*                     |                                     |                         *\n");
    printf("*                     |                                     |                         *\n");
    printf("***************************************************************************************\n");
    wait_ms(10000); // wait 8 seconds. ~ 5 seconds after the user finishes reading everything
    int i = 0;
    while(i<8){
    	current_touch = low_to_high();
    	if(current_touch < 0){
    		avg +=0;
    	}
    	else{
    		avg = avg + current_touch;
    	}
    	wait_ms(20);
    	i++;
    }
    avg = avg/8;
    current_touch = avg;
    if(current_touch < (min_touch+interval)){
    	//small size
    	to_return = 3;
    	strcpy(board_size, small);
    }
    else if(current_touch>=(min_touch+interval) && current_touch<(min_touch + (2*interval))){
      // medium size
    	to_return = 7;
    	strcpy(board_size, medium);
    }

    else if(current_touch>=(min_touch + (2*interval))){
      // large size
    	to_return = 16;
    	strcpy(board_size, large);

    }
    else{
      // do nothing
    	to_return = 100;
    	PRINTF("This happens¯|_(ツ)_|¯. SELECT AGAIN!");
    }

    if (last_touch != current_touch){
    	if(strcmp(board_size,small) == 0){
			PRINTF("\n%s SIZE SELECTED. A 3x3 board has been printed. An AI will play you now!\n", board_size);
		}
    	else if(strcmp(board_size,medium) == 0){
			PRINTF("\n%s SIZE SELECTED. A 7x7 board has been printed. An AI will play you now!\n", board_size);
		}
    	else if(strcmp(board_size,large) == 0){
    	PRINTF("\n%s SIZE SELECTED. A 16x16 board has been printed. An AI will play you now!\n", board_size);
    	}
    }
    return to_return;
}

/*
 * Board initialization with decided width and height using user input
 */
void init_board(int board_width, int board_height){
	//init random seed
	srand(time(NULL));

	int row, col;
	board = malloc(sizeof(char*)*board_width);
	for (row = 0; row<board_size; row++){
		board[row] = malloc(sizeof(char) * board_size);
	}
	empty_squares = board_width * board_height;

	for (row=0; row<board_height; row++){
		for (col=0; col<board_width; col++){
			board[row][col] = ' ';
		}
	}
}

// Deallocates the memory that had been allocated for the board
void free_board(int board_size){
	int row;
	for(row = 0; row<board_size; row++) free(board[row]);
}

// print the board with selected height and width
void print_board(int board_width, int board_height){
	int row, col;
	for (row=1; row<2*board_height; row++){
		if (row % 2 == 0){
			printf("  ---");
			for (col=1; col < board_width; col ++){
				printf("|---");
			}
			printf("\n");
		}
		else{
			printf("  %c  |", board[row/2][0]);

			for (col=1; col<board_width-1; col++){
				printf(" %c |", board[row/2][col]);
			}
			printf("  %c \n", board[row/2][board_width-1]);
		}
	}
}


// Checks the win condition
int check_win(){
	//assumes square grid
	if(empty_squares==0) return -1;

	//brute force check every row / col / diagonal for a string of symbols
	int row, col;
	char current_symbol;
	int streak;

	//check rows
	for (row=0; row<board_size; row++){
		current_symbol = board[row][0];
		if (current_symbol == ' ') continue;

		for (col=1; col<board_size; col++){
			if (board[row][col] == current_symbol){
				streak = 1;
				continue;
			}
			streak = 0;
			break;
		}
		if (streak) {
			printf("Winning row #%d\n", row);
			//player win
			if (current_symbol == X_SYMBOL) return 1;
			//AI win
			else if (current_symbol == O_SYMBOL) return -2;
		}

	}


	//check cols
	for (col=0; col<board_size; col++){
		current_symbol = board[0][col];
		if (current_symbol == ' ') continue;

		for (row=1; row<board_size; row++){
			if (board[row][col] == current_symbol){
				streak = 1;
				continue;
			}
			streak = 0;
			break;
		}
		if (streak) {
			printf("Winning column #%d\n", col);
			//player win
			if (current_symbol == X_SYMBOL) return 1;
			//AI win
			else if (current_symbol == O_SYMBOL) return -2;
		}

	}

	//check diags, only 2
	current_symbol = board[0][0];
	if (current_symbol != ' '){
		for (col=1; col<board_size; col++){
			if(board[col][col] == ' '){
				streak = 0;
				break;
			}
			if(board[col][col] == current_symbol) streak = 1;
			else {
				streak = 0;
				break;
			}
		}
		if (streak) {
			printf("Winning left diagonal\n");
			//player win
			if (current_symbol == X_SYMBOL) return 1;
			//AI win
			else if (current_symbol == O_SYMBOL) return -2;
		}
	}

	//check other diagonal - the hard one
	current_symbol = board[board_size-1][board_size-1];
	if (current_symbol != ' '){
		for (col=(board_size - 1); col>-1; col--){
			row = (board_size-1) - col;
			if(board[row][col] == ' ') {
				streak = 0;
				break;
			}
			if(board[row][col] == current_symbol) streak = 1;
			else {
				streak = 0;
				break;
			}
		}
		if (streak) {
			printf("Winning right diagonal\n");
			//player win
			if (current_symbol == X_SYMBOL) return 1;
			//AI win
			else if (current_symbol == O_SYMBOL) return -2;
		}
	}
	//no one has won and not a draw
	return 0;
}

// Checks if the spot is empty or not
int is_empty(int row, int col){
	if (row > board_size-1 || col > board_size-1 || row < 0 || col < 0){
		printf("That spot is out of bounds sir\n");
		return -1;
	}

	if (board[row][col] != ' '){
		return 0;
	}
	else{
		return 1;
	}

}

//player turn
void x_turn(){
	printf("~~..::PLAYER TURN::..~~\n\n");
	//sequence of actions

	//show board
	print_board(board_size, board_size);

	//take player input
	int row, col;

	printf("Enter the row and column you would like to pick (space separated, 0-indexed)\n");
	scanf("%d %d", &row, &col);

	int empty;
	empty = is_empty(row, col);

	while(empty != 1){
		if (empty != -1)
			printf("Sorry, that spot is already taken\n");
		printf("Enter the row and column you would like to pick (space separated, 0-indexed)\n");
		scanf("%d %d", &row, &col);
		empty = is_empty(row, col);
	}

	//row and col will finally be an empty square
	board[row][col] = X_SYMBOL;
	empty_squares -= 1;
	printf("\n\n\n");
}

//ai turn
void o_turn(int wait_time){
	printf("~~..::AI TURN::..~~\n\n");

	//pick board square
	int empty;
	int row;
	int col;

	empty = 0;

	char progress_bar[6] = "-/-\\|/";

	printf("AI THINKING...\n");
	// robot emoticon
	printf("\n\n\n\n((|O_O|))\n\n\n");

	wait_ms(1000);


	while(!empty){
		row = rand() % board_size;
		col = rand() % board_size;
		empty = is_empty(row, col);
	}
	board[row][col] = O_SYMBOL;


	//show board after (print what move was made)
	printf("AI moved to (%d, %d)\n", row, col);
	//print_board(board_size, board_size);
	empty_squares -= 1;
	printf("---Type any character and press enter to continue---");
	scanf(" %c", progress_bar);
}


//just a wrapper that executes either player turn or AI turn
void take_turn(int player_num, int wait_time){
	if (player_num == PLAYER_TURN){
		x_turn();
	}
	else if (player_num == AI_TURN){
		o_turn(wait_time);
	}
	else{
		printf("FATAL ERROR\n");
		exit(-1);
	}
}



// Decides the game result
int play_game(int wait_time){
	int game_status;
	while(1){
		game_status = check_win();

		if(game_status == 0){
			take_turn(current_turn, wait_time);
			current_turn = !current_turn;
		}
		else if (game_status == 1){
			printf("You Won!\n");
			games_played++;
			// game win condition
			total_wins++;
			break;
		}
		else if(game_status == -1){
			printf("Draw!\n");
			games_played++;
			break;
		}
		else if (game_status == -2){
			printf("You lost against a random AI! YOU SUCK!\n");
			games_played++;
			break;
		}
		else {
			printf("FATAL FLAW DETECTED\n");
			exit(-1);
		}
	}
	return 0;
}


/*
 * @brief   Application entry point.
 */
int main(int argc, char ** argv) {

  /* Init board hardware. */
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
  /* Init FSL debug console. */
  BOARD_InitDebugConsole();
  // initialization functions for slcd and touch sensor
  slcd_init();
  // initializing the touch screen
  touch_sensor_init();
  total_games = 3;
  total_wins = 0;
  current_touch = 0;
  last_touch = 0;
  int wait_time = 0;
  uint32_t slcd_contrast_level = 13;
  // play the total number of games then exit the games
  for(int j = 0; j <= total_games; j++){
	  current_turn = PLAYER_TURN;
	  board_size = touch_range();

	  init_board(board_size, board_size);

	  play_game(wait_time);
	  contrast_control(slcd_contrast_level);
	  slcd_game(total_wins, total_games);
	  free_board(board_size);
  }
  return 0;
}
