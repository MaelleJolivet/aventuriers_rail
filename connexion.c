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

//display the two cities to claim, and the score given (substracted) if success (failure)
void display_objectives(t_objective obj) {
	printf("city 1 : %d\t", obj.city1);
	printf("city 2 : %d\t", obj.city2);
	printf("score : %d\n", obj.score);
}

//ask and register our move
void scanf_move(t_player* me) {
	printf("What's your move?\n");
	printf("1 for drawBlindCard \n2 for drawCard \n3 for drawObjectives \n4 for choseObjectives\n5 for ClaimRoute \n");
	
	int move;
	scanf("%d", &move);
	
	if (move == 1) {
		me->move.type = DRAW_BLIND_CARD;
	}
	
	else if (move == 2) {
		me->move.type = DRAW_CARD;
		printf("color of the card u want in the deck? enter a number\n");
		printf("p=1 w=2 b=3 y=4 o=5 n=6 r=7 g=8 m=9\n");
		int color;
		scanf("%d", &color);
		me->move.drawCard.card = color;
	}
	
	else if (move == 3) {
		me->move.type = DRAW_OBJECTIVES;
	}
	
	else if (move == 4) {
		me->move.type = CHOOSE_OBJECTIVES;
		me->move.chooseObjectives.nbObjectives = 0;
		int infoBis;
		printf("Which objectives do you choose ? send 3 numbers : 0/1 for each one\n");
		for (int i = 0; i < 3; i++) {
			scanf("%d", &infoBis);
			me->move.chooseObjectives.chosen[i] = infoBis;
			if (infoBis == 1) {
				me->move.chooseObjectives.nbObjectives++;
			}
		}
	}
	
	else if (move == 5) {
		me->move.type = CLAIM_ROUTE;
		int info;
		printf("first city: number\n");
		scanf("%d", &info);
		me->move.claimRoute.city1 = info;
		printf("second city: number\n");
		scanf("%d", &info);
		me->move.claimRoute.city2 = info;
		printf("confirm color of the track: p=1 w=2 b=3 y=4 o=5 n=6 g=7 m=8\n");
		scanf("%d", &info);
		me->move.claimRoute.color = info;
		printf("confirm nb of locomotives needed\n");
		scanf("%d", &info);
		me->move.claimRoute.nbLocomotives = info;
	}
	else if (move == 0) {
		me->legalMove = LOOSING_MOVE;
	}
}

//play the chosen move
void play_move(t_player* me, t_game* game) {
	if (me->move.type == DRAW_BLIND_CARD) {
		//is it a legal move + draw the card and stores it if it is
		me->legalMove = drawBlindCard(&me->move.drawBlindCard.card);
		for (int i = 0; i < 9; i++) {
			if (me->move.drawBlindCard.card == i+1) {
				me->hand[i].number++;
			}
		}
		me->nbHand++;
		me->replay = !(me->replay);
		if (me->replay == 1) {
			printf("\nNext round MUST be drawBlindCard or drawCard\n");
		}
	}
	
	else if (me->move.type == DRAW_CARD) {
		me->legalMove = drawCard(me->move.drawCard.card, &game->faceUp[5]);
		for (int i = 0; i < 9; i++) {
			if (me->move.drawCard.card == i+1) {
				me->hand[i].number++;
			}
		}
		me->nbHand++;
		if (me->move.drawCard.card != 9) {
			me->replay = !(me->replay);
		}
		if (me->replay == 1 && me->move.drawCard.card != 9) {
			printf("\nNext round MUST be drawBlindCard or drawCard\n");
		}
	}
	
	else if (me->move.type == DRAW_OBJECTIVES) {
		me->legalMove = drawObjectives(me->move.drawObjectives.objectives);
		for (int i = 0; i < 3; i++) {
			display_objectives(me->move.drawObjectives.objectives[i]);
		}
		printf("\nNext round MUST be choseObjectives\n");
		me->replay = !(me->replay);
	}
	
	else if (me->move.type == CHOOSE_OBJECTIVES) {
		me->legalMove = chooseObjectives(&me->move.chooseObjectives.chosen[3]);
		for (int i = 0; i < 3; i++) {
			if (me->move.chooseObjectives.chosen[i] == 1) {
				me->objectives[i] = me->move.drawObjectives.objectives[i];
				display_objectives(me->objectives[i]);
			}
		}
		me->replay = 0;
	}
}


int main () {
	//Initialization and get map's info
	t_board board = create_game();

	t_color initial_hand[4];
	t_game game = create_map(&board, initial_hand);
	
	//create players and initialize infos
	t_player me = create_player();
	t_player opponent = create_player();
	
	//additional info for me
	//copy
	for (int i = 0; i < 9; i++) {
		me.hand[i].number = 0;
		me.hand[i].color = i+1;
		for (int j = 0; j < 4; j++) {
			if (initial_hand[j] == i+1) {
				me.hand[i].number++;
			}
		}
	}
	
	//Play the game as long as no one loses or wins (LOOSING_MOVE or WINNING_MOVE)
	while (opponent.legalMove == NORMAL_MOVE && me.legalMove == NORMAL_MOVE) {
		printMap();
		
		if (game.current_player == game.me && me.replay == 0) {
			printf("me 1st round\n");
			scanf_move(&me);
			play_move(&me, &game);
			if (me.replay == 1) {
				printMap();
				scanf_move(&me);
				play_move(&me, &game);
			}
			game.current_player = !(game.current_player);
		}
		
		else if (game.current_player == !(game.me) && opponent.replay == 0) {
			opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			if (opponent.replay == 1) {
				printMap();
				opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			}
			game.current_player = !(game.current_player);
		}
	}

	
	if (opponent.legalMove != 0) {
		printf("Bot lost\n");
	}
	if (me.legalMove != 0) {
		printf("Maeru lost\n");
	}
	
	closeConnection();
	
	return 0;
}


