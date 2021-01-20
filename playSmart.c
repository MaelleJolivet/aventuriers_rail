//
//  playSmart.c
//  
//
//  Created by Maëlle Jolivet on 03/01/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TicketToRideAPI.h"
#include "clientAPI.h"
#include "connexion.h"

/* Replace the scanf_move function with functions who play smartly
 
 Game's structure :
 
DONE	 => pick 1 to 3 objectives (draw + chose) : start with 2
		 
DONE	 => find which routes we HAVE to take to complete the objectives :
			-> shorter path ?
			-> higher points ?
			-> fastest (less rounds) ?

		 THEN at each round
		 => if the oppenent took a route, recalculate the shortest route
		 => check if we have completed (an) objective(s)
			-> if yes :
				- if the game is far from over, take a new one
DONE	 => if we have enough cards to take a route we want, we take it
			-> if we can take several routes, choose the highest in points ?
		 => else, we pick cards to help us take the routes we want
			-> faceUp if we are interested
DONE		-> blindCard else
		 
		 
		 Lenght of a route : number of wagons needed ? or number of smaller routes to take ? or both ?
		 
		 Dijkstra algorithm
		 "distanceMini" algorithm
		 
		 
		 
		 function play_move :
		 void play_move(t_player* me, t_game* game, t_route routes[36][36], t_board* board);
		with me->move.type the move chosen
 
 
 
 */

//id closest city unvisited
int distanceMini (int distances[36], int visited[36]) {
	
	int min = 999, indice_min;
	
	for (int i = 0; i < 36; i++) {
		
		if (visited[i] == 0 && distances[i] < min) {
			min = distances[i];
			indice_min = i;
		}
	}
	return indice_min;
}

//shortest path from source to destination
void dijkstra (int source, int destination, t_game* game, t_route routes[36][36], int distances[36], int previous[36]) {
	
	//initialization
	int visited[36];
	
	for (int i = 0; i < 36; i++) {
		distances[i] = 999;
		visited[i] = 0;
	}
	
	distances[source] = 0; //source is at 0 distance from itself
	
	int u = distanceMini(distances, visited);
	
	while (u != destination) {
		visited[u] = 1;
		
		for (int j = 0; j < 36; j++) {
			
			//exists + free = unvisited + current distance from source is bigger than the proposition
			if (visited[j] == 0 && routes[u][j].exist == 1 && routes[u][j].free == 2 && ((distances[u] + routes[u][j].cars) < distances[j])) {
				
				//printf("dij if\n");
				
				distances[j] = distances[u] + routes[u][j].cars;
				
				previous[j] = u;
			}
			
			//i own the route => distance = 0
			else if (routes[u][j].exist == 1 && routes[u][j].free == game->me) {
				distances[j] = 0;
				
				//printf("dij else if\n");

				
				previous[j] = u; //bof mais à voir plus tard
			}
		}
		
		//printf("u for\n");
		u = distanceMini(distances, visited);
	}
	
	/*for (int i = 0; i < 36; i++) {
		printf("%d\t", previous[i]);
	}
	printf("\n\n");*/
	//printf("end\n");

}

//after dijkstra, stores which routes we need in sourceToDest
int storeSourcetoDest (int source, int destination, int previous[36], int sourceToDest[10], t_route routes[36][36]) {
	int city = destination;
	int i = 0;
	sourceToDest[i] = city;
	
	while (city != source) {
		city = previous[city];
		sourceToDest[i+1] = city;
		i++;
	}
	//sourceToDest ex: [6,5,7,8,9] meaning we need the routes (6,5) (5,7) (7,8) (8,9)
	
	for (int j = 0; j < i+1; j++) {
		printf("%d\t", sourceToDest[j]);
	}
	printf("\n");
	
	return i+1; //or i ? i+1 is the real lenght = nb of cities; i would be the number of routes
}




//we DON'T want it anymore
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


int objectives_connected(t_player* me, t_game* game, t_route routes[36][36], int route0[10], int lenght0, int route1[10], int lenght1) {
	
	int boolean = 0;
	
	for (int i = 0; i < lenght0; i++) {
		for (int j = 0; j < lenght1; j++) {
			
			if (route0[i] == route1[j] || (routes[route0[i]][route1[j]].exist == 1 && routes[route0[i]][route1[j]].free == game->me)) {
				boolean = 1;
			}
		}
	}
	return boolean;
}

void prepare_claim(t_player* me, int color, int city_1, int city_2, int loco) {
	
	me->move.type = CLAIM_ROUTE;
	me->move.claimRoute.color = color;
	me->move.claimRoute.city1 = city_1;
	me->move.claimRoute.city2 = city_2;
	me->move.claimRoute.nbLocomotives = loco;
	
}


//smart move to send to play_move
void what_to_play(t_player* me, t_player* opponent, t_game* game, t_route routes[36][36], t_board* board, int route0[10], int lenght0, int* left0, int route1[10], int lenght1, int* left1) {
//, int route2[20], int lenght2, int* left2
	
/*	THEN at each round
	(=> check if we have completed (an) objective(s)
	   -> if yes :
		   - if the game is far from over, take a new one)
	=> if we have enough cards to take a route we want, we take it
	   -> if we can take several routes, choose the highest in points ?
	=> else, we pick cards to help us take the routes we want
	   -> faceUp if we are interested
	   -> blindCard else
 
 
	route0 : {11,10,9,24} means we want routes (11,10) (10,9) (9,24)
	lenght_0 : 3 at first ; then decreases when we take a route
			so 1 route => 2
			   2 routes => 1
			   3 routes => 0 */

	//first version: take a route if we can, take a blindcard else ; TO BE TESTED
	
	//printMap();
	int last_round = 0;
	
	if (opponent->cars <= 2 || me->cars <= 2) {
		last_round = 1;
	}
	
	
	int variable1 = 4, variable2 = 6;
	int connected  = objectives_connected(me, game, routes, route0, lenght0, route1, lenght1);
	
	//try to complete higher scoring objective
	if (*left0 != 0) {
		for (int i = 0; i < lenght0; i++) {
			//O and 1 / then 1 and 2 / then 2 and 3
						
			if (routes[route0[i]][route0[i+1]].free == 2) {
				
				//if enough cards of color1 to take route, we take it
				if ((me->hand[routes[route0[i]][route0[i+1]].color1-1].number >= routes[route0[i]][route0[i+1]].cars) && (me->cars >= routes[route0[i]][route0[i+1]].cars))  {
					
					prepare_claim(me, routes[route0[i]][route0[i+1]].color1, route0[i], route0[i+1], 0);
					(*left0)--;
					play_move(me, game, routes, board);
					return;
				}
				
				//if cards + 1 loco are enough to take route, we take it
				if ((routes[route0[i]][route0[i+1]].color1 != MULTICOLOR) && (me->hand[routes[route0[i]][route0[i+1]].color1-1].number+1 >= routes[route0[i]][route0[i+1]].cars) && (me->hand[8].number >= 1) && (me->cars >= routes[route0[i]][route0[i+1]].cars)) {
					
					prepare_claim(me, routes[route0[i]][route0[i+1]].color1, route0[i], route0[i+1], 1);
					(*left0)--;
					play_move(me, game, routes, board);
					return;
				}
				
				if ((routes[route0[i]][route0[i+1]].color1 == MULTICOLOR) && (routes[route0[i]][route0[i+1]].cars <= 3)) {
					
					for (int j = 1; j < 9; j++) {
						
						if (me->hand[j-1].number >= routes[route0[i]][route0[i+1]].cars + 1) {
							
							printf("NEW SUPER MOVE\n\n");
							prepare_claim(me, j, route0[i], route0[i+1], 0);
							(*left0)--;
							play_move(me, game, routes, board);
							return;
						}
					}
				}
				
				//if enough cards of color2 to take route, we take it
				if (routes[route0[i]][route0[i+1]].color2 != 0 && (me->hand[routes[route0[i]][route0[i+1]].color2-1].number >= routes[route0[i]][route0[i+1]].cars) && (me->cars >= routes[route0[i]][route0[i+1]].cars)) {
					
					prepare_claim(me, routes[route0[i]][route0[i+1]].color2, route0[i], route0[i+1], 0);
					(*left0)--;
					play_move(me, game, routes, board);
					return;
				}
				
				//if cards + 1 loco are enough to take route, we take it
				if ((routes[route0[i]][route0[i+1]].color2 != 0) && (routes[route0[i]][route0[i+1]].color2 != MULTICOLOR) && (me->hand[routes[route0[i]][route0[i+1]].color2-1].number+1 >= routes[route0[i]][route0[i+1]].cars) && (me->hand[8].number >= 1) && (me->cars >= routes[route0[i]][route0[i+1]].cars)) {
					
					prepare_claim(me, routes[route0[i]][route0[i+1]].color2, route0[i], route0[i+1], 1);
					(*left0)--;
					play_move(me, game, routes, board);
					return;
				}
				
				if ((routes[route0[i]][route0[i+1]].color2 != 0) && (routes[route0[i]][route0[i+1]].color2 == MULTICOLOR) && (routes[route0[i]][route0[i+1]].cars <= 3)) {
					
					for (int j = 1; j < 9; j++) {
						
						if (me->hand[j-1].number >= routes[route0[i]][route0[i+1]].cars + 1) {
							
							printf("NEW SUPER MOVE\n\n");
							prepare_claim(me, j, route0[i], route0[i+1], 0);
							(*left0)--;
							play_move(me, game, routes, board);
							return;
						}
					}
				}
			}
		}
	}
	
	//try to complete 2nd scoring objective
	else if (*left0 == 0 && *left1 != 0) {
		for (int i = 0; i < lenght1; i++) {
			//O and 1 / then 1 and 2 / then 2 and 3
						
			if (routes[route1[i]][route1[i+1]].free == 2) {
				
				//if enough cards of color1 to take route, we take it
				if ((me->hand[routes[route1[i]][route1[i+1]].color1-1].number >= routes[route1[i]][route1[i+1]].cars) && (me->cars >= routes[route1[i]][route1[i+1]].cars))  {
					
					prepare_claim(me, routes[route1[i]][route1[i+1]].color1, route1[i], route1[i+1], 0);
					(*left1)--;
					play_move(me, game, routes, board);
					return;
				}
				
				//if cards + 1 loco are enough to take route, we take it
				if ((routes[route1[i]][route1[i+1]].color1 != MULTICOLOR) && (me->hand[routes[route1[i]][route1[i+1]].color1-1].number+1 >= routes[route1[i]][route1[i+1]].cars) && (me->hand[8].number >= 1) && (me->cars >= routes[route1[i]][route1[i+1]].cars)) {
					
					prepare_claim(me, routes[route1[i]][route1[i+1]].color1, route1[i], route1[i+1], 1);
					(*left1)--;
					play_move(me, game, routes, board);
					return;
				}
				
				if ((routes[route1[i]][route1[i+1]].color1 == MULTICOLOR) && (routes[route1[i]][route1[i+1]].cars <= 3)) {
					
					for (int j = 1; j < 9; j++) {
						
						if (me->hand[j-1].number >= routes[route1[i]][route1[i+1]].cars + 1) {
							
							printf("NEW SUPER MOVE\n\n");
							prepare_claim(me, j, route1[i], route1[i+1], 0);
							(*left1)--;
							play_move(me, game, routes, board);
							return;
						}
					}
				}
				
				//if enough cards of color2 to take route, we take it
				if (routes[route1[i]][route0[i+1]].color2 != 0 && (me->hand[routes[route1[i]][route1[i+1]].color2-1].number >= routes[route1[i]][route1[i+1]].cars) && (me->cars >= routes[route1[i]][route1[i+1]].cars)) {
					
					prepare_claim(me, routes[route1[i]][route1[i+1]].color2, route1[i], route1[i+1], 0);
					(*left1)--;
					play_move(me, game, routes, board);
					return;
				}
				
				//if cards + 1 loco are enough to take route, we take it
				if ((routes[route1[i]][route0[i+1]].color2 != 0) && (routes[route1[i]][route1[i+1]].color2 != MULTICOLOR) && (me->hand[routes[route1[i]][route1[i+1]].color2-1].number+1 >= routes[route1[i]][route1[i+1]].cars) && (me->hand[8].number >= 1) && (me->cars >= routes[route1[i]][route1[i+1]].cars)) {
					
					prepare_claim(me, routes[route1[i]][route1[i+1]].color2, route1[i], route1[i+1], 1);
					(*left1)--;
					play_move(me, game, routes, board);
					return;
				}
				
				if ((routes[route1[i]][route0[i+1]].color2 != 0) && (routes[route1[i]][route1[i+1]].color2 == MULTICOLOR) && (routes[route1[i]][route1[i+1]].cars <= 3)) {
					
					for (int j = 1; j < 9; j++) {
						
						if (me->hand[j-1].number >= routes[route1[i]][route1[i+1]].cars + 1) {
							
							printf("NEW SUPER MOVE\n\n");
							prepare_claim(me, j, route1[i], route1[i+1], 0);
							(*left1)--;
							play_move(me, game, routes, board);
							return;
						}
					}
				}
				
			}
		}
	}

	//if we can't take a route to help the objectives, take a "random" route : at least 4/5 cars + try to make a long route OR annoy the opponent
	else {
		for (int line = 0; line < 36; line++) {
			for (int column = 0; column < 36; column++) {
				if (routes[line][column].exist == 1) {
					
					if (*left0 == 0 && *left1 == 0) {
						variable1 = 4;
						variable2 = routes[line][column].cars;
					}
					
					if (last_round == 1) {
						variable1 = 2;
						variable2 = routes[line][column].cars;
					}
					
					if (routes[line][column].free == 2 && routes[line][column].cars >= variable1) {
						
						if ( (line == route0[0] || line == route1[0] || column == route0[0] || column == route1[0] || line == route0[lenght0-1] || line == route1[lenght1-1] || column == route0[lenght0-1] || column == route1[lenght1-1]) && (me->hand[routes[line][column].color1-1].number >= variable2 && (me->cars >= routes[line][column].cars))) {
							
							prepare_claim(me, routes[line][column].color1, line, column, 0);
							play_move(me, game, routes, board);
							printf("ALLONGEMENT\n");
							return;
						}
						
						else if (routes[line][column].color2 != 0 && ((line == route0[0] || line == route1[0] || column == route0[0] || column == route1[0] || line == route0[lenght0-1] || line == route1[lenght1-1] || column == route0[lenght0-1] || column == route1[lenght1-1]) && (me->hand[routes[line][column].color2-1].number >= variable2 && (me->cars >= routes[line][column].cars)))) {
							
							prepare_claim(me, routes[line][column].color2, line, column, 0);
							play_move(me, game, routes, board);
							printf("ALLONGEMENT\n");
							return;
						}
						
						else if ( (line == opponent->move.claimRoute.city1 || line == opponent->move.claimRoute.city2) && me->hand[routes[line][column].color1-1].number >= variable2 && (me->cars >= routes[line][column].cars)) {
							
							prepare_claim(me, routes[line][column].color1, line, column, 0);
							play_move(me, game, routes, board);
							printf("OPPONENT\n");
							return;
						}
						
						else if (routes[line][column].color2 != 0 && ((line == opponent->move.claimRoute.city1 || line == opponent->move.claimRoute.city2) && me->hand[routes[line][column].color2-1].number >= variable2 && (me->cars >= routes[line][column].cars))) {
							
							prepare_claim(me, routes[line][column].color2, line, column, 0);
							play_move(me, game, routes, board);
							printf("OPPONENT 2\n");
							return;
						}
						
						else if (me->hand[routes[line][column].color1-1].number >= variable2 && (me->cars >= routes[line][column].cars)) {
							
							prepare_claim(me, routes[line][column].color1, line, column, 0);
							play_move(me, game, routes, board);
							printf("bonus\n");
							return;
						}
						
						else if (routes[line][column].color2 != 0 && me->hand[routes[line][column].color2 - 1].number >= variable2 && (me->cars >= routes[line][column].cars)) {
							
							prepare_claim(me, routes[line][column].color2, line, column, 0);
							play_move(me, game, routes, board);
							printf("bonus 2\n");
							return;
						}
						/*
						else if (!connected) {
							for (int i = 0; i < lenght0; i++) {
								for (int j = 0; j < lenght1; j++) {
									
									if ((i == column && j == line) || (j == column && i == line)) {
										
										if (me->hand[routes[line][column].color1-1].number >= routes[line][column].cars && (me->cars >= routes[line][column].cars)) {
											
											prepare_claim(me, routes[line][column].color1, line, column, 0);
											play_move(me, game, routes, board);
											printf("CONNECTED\n");
											return;
										}
										
										else if (me->hand[routes[line][column].color2-1].number >= routes[line][column].cars && (me->cars >= routes[line][column].cars)) {
											
											prepare_claim(me, routes[line][column].color2, line, column, 0);
											play_move(me, game, routes, board);
											printf("CONNECTED\n");
											return;
										}
									}
								}
							}
						}*/
					}
				}
			}
		}
	}
	
	if (last_round != 0 || (last_round == 0 && me->cars != 0)) {
		printf("BLIND CARD 1\n");
		//if we haven't been able to take a route, take a blindcard
		me->move.type = DRAW_BLIND_CARD;
		play_move(me, game, routes, board);
		if (me->legalMove == NORMAL_MOVE) {
			printf("BLIND CARD 2\n");
			me->move.type = DRAW_BLIND_CARD;
			play_move(me, game, routes, board);
			printf("2 BLIND CARD TAKEN\n");
		}
	}
	
}


//play the chosen move && update my info
void play_move(t_player* me, t_game* game, t_route routes[36][36], t_board* board) {
	
	//DRAW_BLIND_CARD
	if (me->move.type == DRAW_BLIND_CARD) {
		//is it a legal move + draw the card and stores it if it is
		me->legalMove = drawBlindCard(&me->move.drawBlindCard.card);
		for (int i = 0; i < 9; i++) {
			//i+1 is the real color number (ex: purple is 1, but stored in hand[0])
			if (me->move.drawBlindCard.card == i+1) {
				me->hand[i].number++;
			}
		}
		me->nbHand++;
		me->replay = !(me->replay);
		
		/*if (me->replay == 1) {
			printf("\nNext round MUST be drawBlindCard or drawCard\n");
		}*/
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
		/*if (me->replay == 1 && me->move.drawCard.card != 9) {
			printf("\nNext round MUST be drawBlindCard or drawCard\n");
		}*/
	}
	
	//DRAW_OBJECTIVES
	else if (me->move.type == DRAW_OBJECTIVES) {
		me->legalMove = drawObjectives(me->move.drawObjectives.objectives);
		for (int i = 0; i < 3; i++) {
			display_objectives(&me->move.drawObjectives.objectives[i]);
			me->temporary_obj[i] = me->move.drawObjectives.objectives[i];
		}
		//printf("\nNext round MUST be choseObjectives\n");
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
		
		me->hand[me->move.claimRoute.color - 1].number -= used_color_cars;
		
		me->hand[8].number -= used_loco;
		
		me->nbHand -= (used_loco + used_color_cars);
		
		//display_my_info(me);
		me->replay = 0;
	}
}




/*
else if ((*left2 != 0 && *left0 == 0) || (*left2 != 0 && *left1 == 0)) {
	for (int i = 0; i < lenght2; i++) {
		//O and 1 / then 1 and 2 / then 2 and 3
					
		if (routes[route2[i]][route2[i+1]].free == 2) {
			
			if ((me->hand[routes[route2[i]][route2[i+1]].color1-1].number >= routes[route2[i]][route2[i+1]].cars) && (me->cars >= routes[route2[i]][route2[i+1]].cars))  {
				
				me->move.type = CLAIM_ROUTE;
				me->move.claimRoute.city1 = route2[i];
				me->move.claimRoute.city2 = route2[i+1];
				me->move.claimRoute.color = routes[route2[i]][route2[i+1]].color1;
				me->move.claimRoute.nbLocomotives = 0;
				(*left2)--;
				play_move(me, game, routes, board);
				return;
			}
			
			else if ((routes[route0[i]][route0[i+1]].color1 != 9) && (me->hand[routes[route2[i]][route2[i+1]].color1-1].number+1 >= routes[route2[i]][route2[i+1]].cars) && (me->hand[8].number >= 1) && (me->cars >= routes[route2[i]][route2[i+1]].cars)) {
				
				printf("city1 %d city2 %d\n", route1[i], route1[i+1]);

				me->move.type = CLAIM_ROUTE;
				me->move.claimRoute.city1 = route2[i];
				me->move.claimRoute.city2 = route2[i+1];
				me->move.claimRoute.color = routes[route2[i]][route2[i+1]].color1;
				me->move.claimRoute.nbLocomotives = 1;
				(*left2)--;
				play_move(me, game, routes, board);
				return;
			}

			else if (routes[route2[i]][route0[i+1]].color2 != 0 && (me->hand[routes[route2[i]][route2[i+1]].color2-1].number >= routes[route2[i]][route2[i+1]].cars) && (me->cars >= routes[route2[i]][route2[i+1]].cars)) {
				me->move.type = CLAIM_ROUTE;
				me->move.claimRoute.city1 = route2[i];
				me->move.claimRoute.city2 = route2[i+1];
				me->move.claimRoute.color = routes[route2[i]][route2[i+1]].color2;
				me->move.claimRoute.nbLocomotives = 0;
				(*left2)--;
				play_move(me, game, routes, board);
				return;
			}
		}
	}
}*/
