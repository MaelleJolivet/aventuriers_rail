//
//  connexion.c
//  
//
//  Created by Maëlle Jolivet on 30/11/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TicketToRideAPI.h"
#include "clientAPI.h"
#include "connexion.h"

t_board create_game() {
	connectToServer("li1417-56.members.linode.com",1234,"Maeruh");
	char* gameName = malloc(50*sizeof(char));
	t_board board;
	waitForT2RGame("TRAINING DO_NOTHING map=small", gameName, &board.nbCities, &board.nbTracks);
	board.tracks = malloc(5*(board.nbTracks)*sizeof(int));
	free(gameName);
	return board;
}


t_game create_map(t_board* board, t_color initial_hand[]) {
	printf("cities : %d, tracks : %d\n", board->nbCities, board->nbTracks);
	t_game game;
	game.me = getMap(board->tracks, game.faceUp, initial_hand);
	game.board = *board;
	game.current_player = 0;
	return game;
}

t_player create_player() {
	t_player player;
	player.cars = 45;
	player.nbHand = 4;
	player.hand = malloc(200*sizeof(t_color));
	player.objectives = malloc(10*sizeof(t_objective));
	return player;
}


int main () {
	//Initialization and get map's info
	
	t_board board = create_game();

	t_color initial_hand[4];
	t_game game = create_map(&board, initial_hand);
	
	t_player me = create_player();

	t_player opponent;
	opponent.cars = 45;
	
	
	t_return_code moveOpponent = NORMAL_MOVE;
	t_return_code moveMe = NORMAL_MOVE;
	t_move opp_move;
	t_color card;
	int replay_opp = 0;
	int replay_me = 0;
	
	//Start game
	while (moveOpponent == NORMAL_MOVE && moveMe == NORMAL_MOVE) {
		printMap();
		if (game.current_player == game.me && replay_me == 0) {
			moveMe = drawBlindCard(&card);
			replay_me = 1;
		}
		else if (game.current_player == game.me && replay_me == 1) {
			moveMe = drawBlindCard(&card);
			replay_me = 0;
			game.current_player = !(game.current_player);
		}
		else if (game.current_player == !(game.me) && replay_opp == 0) {
			moveOpponent = getMove(&opp_move, &replay_opp);
		}
		else if (game.current_player == !(game.me) && replay_opp == 1) {
			moveOpponent = getMove(&opp_move, &replay_opp);
			game.current_player = !(game.current_player);
		}
		printf("jezano\n");
	}

	
	if (moveOpponent != 0) {
		printf("Bot lost\n");
	}
	if (moveMe != 0) {
		printf("Maeru lost\n");
	}
	
	free(me.hand);
	free(me.objectives);
	closeConnection();
	
	return 0;
}


