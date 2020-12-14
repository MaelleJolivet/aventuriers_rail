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
	t_objective objectives[3]; //malloc à 10 ?
	t_return_code legalMove;
	t_move move;
} t_player;

typedef struct {
	int current_player;
	int me;
	t_color faceUp[5];
	t_board board;
	t_player players[2];
} t_game;


//initialize connexion and create board with map + nb of cities + b of tracks
t_board create_game();

//initialize the map : 1st player, cards face up, our hand, fill the map with the cities
t_game create_map(t_board* board, t_color initial_hand[]);

//create a player
t_player create_player();

//ask and register our move
void scanf_move(t_player* me);

//play the chosen move
void play_move(t_player* me, t_game* game);

//display the two cities to claim, and the score given (substracted) if success (failure)
void display_objectives(t_objective obj);
