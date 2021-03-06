//
//  connexion.h
//  
//
//  Created by Maëlle Jolivet on 10/12/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TicketToRideAPI.h"


typedef struct {
	int nbCities;
	int nbTracks;
	int* tracks;
} t_board;

typedef struct {
	t_color color;
	int number;
} t_hand;

typedef struct {
	int cars;
	int nbHand;
	int nbObjectives;
	int replay;
	t_hand hand[9]; //9 different possible card colors
	t_objective temporary_obj[3]; //stores drawn objectives array, before chosing the ones to keep in objectives
	t_objective* objectives; //malloc
	t_return_code legalMove;
	t_move move;
} t_player;

typedef struct {
	int exist;
	int city1;
	int city2;
	int cars; //lenght
	int color1;
	int color2;
	int free; //0, 1 or 2 (ex: i own the route : free = game.me ; the route is free free = 2)
} t_route;

typedef struct {
	int current_player;
	int me;
	t_color faceUp[5];
	t_board board;
	t_player players[2];
} t_game;


//initialize connexion and create board with map + nb of cities + nb of tracks
t_board create_game();

//initialize the map : 1st player, cards face up, our hand, fill the map with the cities
t_game create_map(t_board* board, t_color initial_hand[]);

//create and initialize a player
t_player create_player();

//display the two cities to claim, and the score given (substracted) if success (failure)
void display_objectives(t_objective* obj);

//print my numbers of cars, cards and objectives
void display_my_info(t_player* me);

//print a route's info
void display_route(t_route* route);

//fills the 2D array of t_route (to get all routes' info)
void array_routes(t_route routes[36][36], t_board* board);

//ask and register our move
void scanf_move(t_player* me);

//unused, would return bool 1 if our 2 objectives or connected (1 city or 1 route in commun)
int objectives_connected(t_player* me, t_game* game, t_route routes[36][36], int route0[10], int lenght0, int route1[10], int lenght1);

//to prepare the move claimroute
void prepare_claim(t_player* me, int color, int city_1, int city_2, int loco);

//smart move to send to play_move
void what_to_play(t_player* me, t_player* opponent, t_game* game, t_route routes[36][36], t_board* board, int route0[10], int lenght0, int* left0, int route1[10], int lenght1, int* left1);

//play the chosen move
void play_move(t_player* me, t_game* game, t_route routes[36][36], t_board* board);

//id closest city unvisited
int distanceMini (int distances[36], int visited[36]);

//shortest path from source to destination
void dijkstra (int source, int destination, t_game* game, t_route routes[36][36], int distances[36], int previous[36]);

//after dijkstra, stores which routes we need in sourceToDest, returns size of sourceToDest (10 being the maximum)
int storeSourcetoDest (int source, int destination, int previous[36], int sourceToDest[10], t_route routes[36][36]);
