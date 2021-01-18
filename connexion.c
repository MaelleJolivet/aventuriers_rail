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
	
	//after dijkstra, stores the routes we want for each objectives
	//for me.objectives[0] and me.objectives[1]
	int lenght0, lenght1, lenght2; //won't be changed: size of array
	int left0, left1, left2; //will change: nb of routes still to be taken
	int routes_objective_0[20], routes_objective_1[20], routes_objective_2[20];
	int distances_0[36], distances_1[36], distances_2[36];
	int previous_0[36], previous_1[36], previous_2[36];
	

	//Play the game as long as no one loses or wins (LOOSING_MOVE or WINNING_MOVE)
	while (opponent.legalMove == NORMAL_MOVE && me.legalMove == NORMAL_MOVE) {
		printMap();
		
		//begin smart play => first 2 moves HAVE to be draw&choose objectives
		if (game.current_player == game.me && me.replay == 0) {
			
			if (first_round == 1) {
				//printMap();
				printf("1st move is Draw Objectives\n");
				me.move.type = DRAW_OBJECTIVES;
				play_move(&me, &game, routes, &board);
				
				me.move.type = CHOOSE_OBJECTIVES;
				me.move.chooseObjectives.nbObjectives = 0;
				me.move.chooseObjectives.chosen[0] = 1;
				me.move.chooseObjectives.chosen[1] = 1;
				me.move.chooseObjectives.chosen[2] = 0;
				play_move(&me, &game, routes, &board);
				
				first_round = 0;
				
				int first_dude, second_dude;
				if (me.objectives[0].score >= me.objectives[1].score) {
					first_dude = 0;
					second_dude = 1;
				}
				else {
					first_dude = 1;
					second_dude = 0;
				}
				
				
				//immediatly run the shorter paths algo to found which routes we want
				
				//1st objective
				dijkstra(me.objectives[first_dude].city1, me.objectives[first_dude].city2, &game, routes, distances_0, previous_0);
				
				lenght0 = storeSourcetoDest(me.objectives[first_dude].city1, me.objectives[first_dude].city2, previous_0, routes_objective_0, routes);
				left0 = lenght0 - 1;
				
				
				//2nd objective
				dijkstra(me.objectives[second_dude].city1, me.objectives[second_dude].city2, &game, routes, distances_1, previous_1);
				
				lenght1 = storeSourcetoDest(me.objectives[second_dude].city1, me.objectives[second_dude].city2, previous_1, routes_objective_1, routes);
				left1 = lenght1 - 1;
				
				/*//3rd objective
				dijkstra(me.objectives[2].city1, me.objectives[2].city2, &game, routes, distances_2, previous_2);
				
				lenght2 = storeSourcetoDest(me.objectives[2].city1, me.objectives[2].city2, previous_2, routes_objective_2, routes);
				left2 = lenght2 - 1;*/
			}
			
			else {				
				/*if (left0 == 0) {
					me.nbObjectives--;
					printMap();
					//printf("1st move is Draw Objectives\n");
					me.move.type = DRAW_OBJECTIVES;
					play_move(&me, &game, routes, &board);
					
					me.move.type = CHOOSE_OBJECTIVES;
					me.move.chooseObjectives.nbObjectives = 0;
					me.move.chooseObjectives.chosen[0] = 0;
					me.move.chooseObjectives.chosen[1] = 0;
					me.move.chooseObjectives.chosen[2] = 1;
					play_move(&me, &game, routes, &board);
										
					//immediatly run the shorter paths algo to found which routes we want
					
					//1st objective
					dijkstra(me.objectives[0].city1, me.objectives[0].city2, &game, routes, distances_0, previous_0);
					
					lenght0 = storeSourcetoDest(me.objectives[0].city1, me.objectives[0].city2, previous_0, routes_objective_0, routes);
					left0 = lenght0 - 1;
				}
				else {*/
					what_to_play(&me, &game, routes, &board, routes_objective_0, lenght0, &left0, routes_objective_1, lenght1, &left1);
				//, routes_objective_2, lenght2, &left2
				//}
			}
			
			//printf("NEXT ROUND\n\n");
			game.current_player = !(game.current_player);

		}
		
		else if (game.current_player == !(game.me) && opponent.replay == 0) {
			//printMap();

			opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			
			if (opponent.move.type == 1) {
				routes[opponent.move.claimRoute.city1][opponent.move.claimRoute.city2].free = !(game.me);
				routes[opponent.move.claimRoute.city2][opponent.move.claimRoute.city1].free = !(game.me);
				
				for (int i = 0; i < lenght0; i++) {
					
					if (left0 != 0 && ((routes_objective_0[i] == opponent.move.claimRoute.city1 && routes_objective_0[i+1] == opponent.move.claimRoute.city2) || (routes_objective_0[i] == opponent.move.claimRoute.city2 && routes_objective_0[i+1] == opponent.move.claimRoute.city1))) {
						left0 = 0;
						printf("CAN'T COMPLETE OBJECTIVE 0\n\n");
					}
					
					else if (left1 != 0 && ((routes_objective_1[i] == opponent.move.claimRoute.city1 && routes_objective_1[i+1] == opponent.move.claimRoute.city2) || (routes_objective_1[i] == opponent.move.claimRoute.city2 && routes_objective_1[i+1] == opponent.move.claimRoute.city1))) {
						left1 = 0;
						printf("CAN'T COMPLETE OBJECTIVE 1\n\n");
					}
					
					/*else if ((routes_objective_2[i] == opponent.move.claimRoute.city1 && routes_objective_2[i+1] == opponent.move.claimRoute.city2) || (routes_objective_2[i] == opponent.move.claimRoute.city2 && routes_objective_2[i+1] == opponent.move.claimRoute.city1)) {
						left2 = 0;
						printf("CAN'T COMPLETE OBJECTIVE 2\n\n");
					}*/
				}

			}
			
			if (opponent.replay == 1) {
				opponent.legalMove = getMove(&opponent.move, &opponent.replay);
			}
			
			game.current_player = !(game.current_player);
		}
	}
/*
	if (opponent.legalMove != 0) {
		printf("Bot lost\n");
	}
	if (me.legalMove != 0) {
		printf("Maeru lost\n");
	}*/
	
	free(board.tracks);
	free(me.objectives);
	closeConnection();
	
	return 0;
}


