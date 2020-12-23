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

//initialize connexion and create board with map + nb of cities + nb of tracks
t_board create_game() {
	connectToServer("li1417-56.members.linode.com",5678,"Maeru");
	char* gameName = malloc(50*sizeof(char));
	t_board board;
	//map=small DO_NOTHING
	waitForT2RGame("TRAINING PLAY_RANDOM timeout=100", gameName, &board.nbCities, &board.nbTracks);
	printf("gameName : %s\n", gameName);
	board.tracks = malloc(5*(board.nbTracks)*sizeof(int));
	free(gameName);
	return board;
}

//initialize the map : 1st player, cards face up, our hand, fill the map with the cities
t_game create_map(t_board* board, t_color initial_hand[]) {
	printf("cities : %d, tracks : %d\n", board->nbCities, board->nbTracks);
	t_game game;
	game.me = getMap(board->tracks, game.faceUp, initial_hand);
	
	//info for first route
	for (int i = 0; i < 5; i++) {
		//printf("%d\t", board->tracks[i]);
	}
	
	game.board = *board;
	game.current_player = 0;
	return game;
}

//display the two cities to claim, and the score given (substracted) if success (failure)
void display_objectives(t_objective obj) {
	printf("city 1 : %d\t", obj.city1);
	printf("city 2 : %d\t", obj.city2);
	printf("score : %d\n", obj.score);
}

//print a route's info
void display_route(t_route route) {
	printf("city 1 : %d\t", route.city1);
	printf("city 2 : %d\n", route.city2);
	printf("cars needed : %d\t", route.cars);
	printf("first color : %d\t", route.color1);
	printf("second color : %d\t", route.color2);
	printf("whom : %d\n", route.free);
}

//fills the 2D array of t_route pointers (to get all routes' info)
void array_routes(t_route* routes[35][35], t_board* board) {
	for (int line = 0; line < 36; line++) {
		for (int column = 0; column < 36; column++) {
			routes[line][column] = NULL;
		}
	}
	
	t_route route_temp;
	int city1, city2;
	// [(city1, city2, cars, color1, color2), (city1, city2, cars, color1, color2),...]
	
	for (int i = 0; i < 5*board->nbTracks; i+=5) {
		city1 = board->tracks[i];
		city2 = board->tracks[i+1];
		
		route_temp.city1 = city1;
		//printf("city1 %d\t", route_temp.city1);

		route_temp.city2 = city2;
		//printf("city2 %d\t", route_temp.city2);

		route_temp.cars = board->tracks[i+2];
		//printf("cars %d\t", route_temp.cars);

		route_temp.color1 = board->tracks[i+3];
		//printf("color1 %d\t", route_temp.color1);

		route_temp.color2 = board->tracks[i+4];
		//printf("color2 %d\n", route_temp.color2);

		route_temp.free = 2;
		
		routes[city1][city2] = &route_temp;
		routes[city2][city1] = &route_temp;
	}
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
		printf("Which objectives do you choose ? send 3 numbers : 0/1 for each one\n");
		scanf("%d %d %d", &me->move.chooseObjectives.chosen[0], &me->move.chooseObjectives.chosen[1], &me->move.chooseObjectives.chosen[2]);
	}
	
	else if (move == 5) {
		for (int i = 0; i < me->nbObjectives; i++) {
			display_objectives(me->objectives[i]);
		}
		me->move.type = CLAIM_ROUTE;
		printf("first city, second city, color of the track, extra locomotives ?\n");
		scanf("%d %d %d %d", &me->move.claimRoute.city1, &me->move.claimRoute.city2, &me->move.claimRoute.color, &me->move.claimRoute.nbLocomotives);
	}
	
	else if (move == 0) {
		me->legalMove = LOOSING_MOVE;
	}
}

//play the chosen move
void play_move(t_player* me, t_game* game) {
	
	//DRAW_BLIND_CARD
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
	
	//DRAW_CARD
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
	
	//DRAW_OBJECTIVES
	else if (me->move.type == DRAW_OBJECTIVES) {
		me->legalMove = drawObjectives(me->move.drawObjectives.objectives);
		for (int i = 0; i < 3; i++) {
			display_objectives(me->move.drawObjectives.objectives[i]);
			me->temporary_obj[i] = me->move.drawObjectives.objectives[i];
		}
		printf("\nNext round MUST be choseObjectives\n");
		me->replay = !(me->replay);
	}
	
	//CHOOSE_OBJECTIVES
	else if (me->move.type == CHOOSE_OBJECTIVES) {
		me->legalMove = chooseObjectives(me->move.chooseObjectives.chosen);
		
		for (int i = me->nbObjectives; i < me->nbObjectives+3; i++) {
			if (me->move.chooseObjectives.chosen[i] == 1) {
				me->move.chooseObjectives.nbObjectives++;
				me->objectives[i] = me->temporary_obj[i];
				display_objectives(me->objectives[i]);
			}
		}
		me->nbObjectives += me->move.chooseObjectives.nbObjectives;
		me->replay = 0;
	}
	
	//CLAIM_ROUTE
	else if (me->move.type == CLAIM_ROUTE) {
		me->legalMove = claimRoute(me->move.claimRoute.city1, me->move.claimRoute.city2, me->move.claimRoute.color, me->move.claimRoute.nbLocomotives);
		
	}
}


int main () {
	//Initialization and get map's info
	t_board board = create_game();

	t_color initial_hand[4];
	t_game game = create_map(&board, initial_hand);
	
	//array 2D of t_route pointers => 36 cities
	//ex: routes[0][8] & routes[8][0] are pointers to the same t_route, who contains all the route's info
	//this array needs to be filled
	t_route* routes[35][35];
	array_routes(routes, &board);
		
	//create players and initialize infos
	t_player me = create_player();
	t_player opponent = create_player();
	
	//additional info for me
	me.objectives = malloc(10*sizeof(t_objective));
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
	
	free(board.tracks);
	free(me.objectives);
	closeConnection();
	
	return 0;
}


