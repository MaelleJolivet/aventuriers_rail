//
//  connexionn.c
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
	int cars;
	int nbHand;
	int nbObjectives;
	int replay;
	t_color* hand;
	t_objective* objectives; //malloc à 10 ?
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


t_board create_game();

t_game create_map(t_board* board, t_color initial_hand[]);
