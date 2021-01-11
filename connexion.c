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
			display_objectives(&me->objectives[i]);
		}
		me->move.type = CLAIM_ROUTE;
		printf("first city, second city, color of the track, extra locomotives ?\n");
		scanf("%d %d %d %d", &me->move.claimRoute.city1, &me->move.claimRoute.city2, &me->move.claimRoute.color, &me->move.claimRoute.nbLocomotives);
	}
	
	else if (move == 0) {
		me->legalMove = LOOSING_MOVE;
	}
}

//play the chosen move && update my info
void play_move(t_player* me, t_game* game, t_route routes[36][36], t_board* board) {
	
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
			display_objectives(&me->move.drawObjectives.objectives[i]);
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
				display_objectives(&me->objectives[i]);
			}
		}
		me->nbObjectives += me->move.chooseObjectives.nbObjectives;
		me->replay = 0;
	}
	
	//CLAIM_ROUTE
	else if (me->move.type == CLAIM_ROUTE) {
		int city1 = me->move.claimRoute.city1;
		int city2 = me->move.claimRoute.city2;
		
		me->legalMove = claimRoute(city1, city2, me->move.claimRoute.color, me->move.claimRoute.nbLocomotives);
		
		routes[city1][city2].free = game->me;
		routes[city2][city1].free = game->me;

		
		int used_color_cars = routes[city1][city2].cars;
		int used_loco = me->move.claimRoute.nbLocomotives;
		
		me->cars -= used_color_cars;
		me->hand[me->move.claimRoute.color].number -= used_color_cars;
		me->hand[me->move.claimRoute.nbLocomotives].number -= used_loco;
		me->nbHand -= (used_loco + used_color_cars);
		display_my_info(me);
		me->replay = 0;
	}
}


int main () {
	//Initialization and get map's info
	t_board board = create_game();

	t_color initial_hand[4];
	t_game game = create_map(&board, initial_hand);
	
	//array 2D of t_route => 36 cities
	//ex: routes[0][8] & routes[8][0] are the same t_route, who contains all the route's info
	t_route routes[36][36];
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
	
	int first_round = 1;

	//Play the game as long as no one loses or wins (LOOSING_MOVE or WINNING_MOVE)
	while (opponent.legalMove == NORMAL_MOVE && me.legalMove == NORMAL_MOVE) {
		
		//begin smart play => first 2 moves HAVE to be draw&choose objectives
		if (game.current_player == game.me && me.replay == 0) {
			if (first_round == 1) {
				printMap();
				printf("1st move is Draw Objectives\n");
				me.move.type = DRAW_OBJECTIVES;
				play_move(&me, &game, routes, &board);
				
				printMap();
				printf("2nd move is Choose Objectives (at least 2)\n");
				scanf_move(&me);
				play_move(&me, &game, routes, &board);
				
				first_round = 0;
			}
			
			else {
				printMap();
				printf("me 1st round\n");
				scanf_move(&me);
				play_move(&me, &game, routes, &board);
				if (me.replay == 1) {
					printMap();
					scanf_move(&me);
					play_move(&me, &game, routes, &board);
				}
			}
			
			game.current_player = !(game.current_player);
		}
		
		else if (game.current_player == !(game.me) && opponent.replay == 0) {
			printMap();
			opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			if (opponent.replay == 1) {
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


