//
//  createDisplayGame.c
//  
//
//  Created by Maëlle Jolivet on 25/12/2020.
//

#include <stdio.h>
#include "TicketToRideAPI.h"
#include "clientAPI.h"
#include "connexion.h"

//initialize connexion and create board with map + nb of cities + nb of tracks
t_board create_game() {
	connectToServer("li1417-56.members.linode.com",3456,"Maeru");
	char* gameName = malloc(50*sizeof(char));
	t_board board;
	//map=small DO_NOTHING PLAY_RANDOM NICE_BOT
	waitForT2RGame("TRAINING NICE_BOT timeout=200", gameName, &board.nbCities, &board.nbTracks);
	printf("gameName : %s\n", gameName);
	board.tracks = malloc(5*(board.nbTracks)*sizeof(int));
	free(gameName);
	return board;
}

//initialize the map : 1st player, cards face up, our hand, fill the map with the cities
t_game create_map(t_board* board, t_color initial_hand[]) {
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
void display_objectives(t_objective* obj) {
	printf("city 1 : %d  city 2 : %d  score : %d\n", obj->city1, obj->city2, obj->score);
}

//print my numbers of cars, cards and objectives
void display_my_info(t_player* me) {
	printf("cars : %d  nbHand : %d  nbObjectives : %d\n", me->cars, me->nbHand, me->nbObjectives);
}

//print a route's info
void display_route(t_route* route) {
	if (route->exist) {
		printf("city 1 : %d  city 2 : %d\n", route->city1, route->city2);
		printf("cars needed : %d  first color : %d", route->cars, route->color1);
		printf("  second color : %d  whom : %d\n", route->color2, route->free);
	}
	else {
		printf("route doesn't exist\n");
	}
}

//fills the 2D array of t_route (to get all routes' info)
void array_routes(t_route routes[36][36], t_board* board) {
	for (int line = 0; line < 36; line++) {
		for (int column = 0; column < 36; column++) {
			routes[line][column].exist = 0;
		}
	}
	
	int city1, city2; // [(city1, city2, cars, color1, color2), (city1, city2, cars, color1, color2),...]
	
	for (int i = 0; i < 5*board->nbTracks; i+=5) {
		
		city1 = board->tracks[i];
		city2 = board->tracks[i+1];
		routes[city1][city2].exist = 1;
		routes[city1][city2].city1 = city1;
		routes[city1][city2].city2 = city2;
		routes[city1][city2].cars = board->tracks[i+2];
		routes[city1][city2].color1 = board->tracks[i+3];
		routes[city1][city2].color2 = board->tracks[i+4];
		routes[city1][city2].free = 2;
		
		routes[city2][city1] = routes[city1][city2];
		
	}
}



