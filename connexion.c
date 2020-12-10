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

//initialize connexion and create board with map + nb of cities + b of tracks
t_board create_game() {
	connectToServer("li1417-56.members.linode.com",1234,"Maeruh");
	char* gameName = malloc(50*sizeof(char));
	t_board board;
	waitForT2RGame("TRAINING DO_NOTHING map=small", gameName, &board.nbCities, &board.nbTracks);
	board.tracks = malloc(5*(board.nbTracks)*sizeof(int));
	free(gameName);
	return board;
}

//initialize the map : 1st player, cards face up, our hand, fill the map with the cities
t_game create_map(t_board* board, t_color initial_hand[]) {
	printf("cities : %d, tracks : %d\n", board->nbCities, board->nbTracks);
	t_game game;
	game.me = getMap(board->tracks, game.faceUp, initial_hand);
	game.board = *board;
	game.current_player = 0;
	return game;
}

//create a player
t_player create_player() {
	t_player player;
	player.cars = 45;
	player.nbHand = 4;
	player.nbObjectives = 0;
	player.replay = 0;
	player.legalMove = NORMAL_MOVE;
	return player;
}


int main () {
	//Initialization and get map's info
	
	t_board board = create_game();

	t_color initial_hand[4];
	t_game game = create_map(&board, initial_hand);
	
	t_player me = create_player();
	t_player opponent = create_player();
	me.hand = malloc(200*sizeof(t_color));
	me.objectives = malloc(10*sizeof(t_objective));

	
	t_color card;
	
	//Start game
	while (opponent.legalMove == NORMAL_MOVE && me.legalMove == NORMAL_MOVE) {
		printMap();
		
		if (game.current_player == game.me && me.replay == 0) {
			me.legalMove = drawBlindCard(&card);
			me.replay = 1;
		}
		
		else if (game.current_player == game.me && me.replay == 1) {
			me.legalMove = drawBlindCard(&card);
			me.replay = 0;
			game.current_player = !(game.current_player);
		}
		
		else if (game.current_player == !(game.me) && opponent.replay == 0) {
			opponent.legalMove = getMove(&opponent.move, &opponent.replay);
		}
		
		else if (game.current_player == !(game.me) && opponent.replay == 1) {
			opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			game.current_player = !(game.current_player);
		}
		
		printf("jezano\n");
	}

	
	if (opponent.legalMove != 0) {
		printf("Bot lost\n");
	}
	if (me.legalMove != 0) {
		printf("Maeru lost\n");
	}
	
	free(me.hand);
	free(me.objectives);
	closeConnection();
	
	return 0;
}


