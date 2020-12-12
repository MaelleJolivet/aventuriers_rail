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
	connectToServer("li1417-56.members.linode.com",1234,"Maeru");
	char* gameName = malloc(50*sizeof(char));
	t_board board;
	waitForT2RGame("TRAINING DO_NOTHING map=small timeout=100", gameName, &board.nbCities, &board.nbTracks);
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

//ask and register our move
void scanf_move(t_player* me) {
	printf("What's your move?\n");
	printf("b for drawBlindCard \nr for ClaimRoute \nd for drawCard \no for drawwObjectives \nc for choseObjectives\n");
	t_move me_move;
	char move = '\0';
	
	scanf("%c", &move);
	
	if (move == 'b') {
		me_move.type = DRAW_BLIND_CARD;
	}
	else if (move == 'd') {
		me_move.type = DRAW_CARD;
		printf("color of the card u want in the deck? enter a number\n");
		printf("p=1 w=2 b=3 y=4 o=5 n=6 g=7 m=8\n");
		int color;
		scanf("%d", &color);
		me_move.drawCard.card = color;
	}
	else if (move == 'o') {
		me_move.type = DRAW_OBJECTIVES;
	}
	else if (move == 'c') {
		me_move.type = CHOOSE_OBJECTIVES;
	}
	else if (move == 'r') {
		me_move.type = CLAIM_ROUTE;
		int info;
		printf("first city: number\n");
		scanf("%d", &info);
		me_move.claimRoute.city1 = info;
		printf("second city: number\n");
		scanf("%d", &info);
		me_move.claimRoute.city2 = info;
		printf("confirm color of the track: p=1 w=2 b=3 y=4 o=5 n=6 g=7 m=8\n");
		scanf("%d", &info);
		me_move.claimRoute.color = info;
		printf("confirm nb of locomotives needed\n");
		scanf("%d", &info);
		me_move.claimRoute.nbLocomotives = info;
	}
	me->move = me_move;
}

//void play_move()


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
			scanf_move(&me);
		}
		
		else if (game.current_player == game.me && me.replay == 1) {
			me.legalMove = drawBlindCard(&card);
			me.replay = 0;
			game.current_player = !(game.current_player);
			scanf_move(&me);
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


