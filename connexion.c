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


int main () {
	connectToServer("li1417-56.members.linode.com",1234,"Maeru");
	
	char* gameName = malloc(50*sizeof(char));
	int nbCities, nbTracks;
	waitForT2RGame("TRAINING DO_NOTHING start=0", gameName, &nbCities, &nbTracks);
	
	printf("cities : %d, tracks : %d\n", nbCities, nbTracks);
	
	int tracks[5*nbTracks];
	t_color faceUp[5];
	t_color cards[4];
	printf("%d\n", getMap(tracks, faceUp, cards));
	
	t_return_code opponent = NORMAL_MOVE;
	t_return_code me = NORMAL_MOVE;
	t_move opp_move;
	t_color card;
	int replay_opp = 0;
	int replay_me = 0;
	int player = 0;
	printMap();

	
	while (opponent == NORMAL_MOVE && me == NORMAL_MOVE) {
		if (player == 0 && replay_me == 0) {
			me = drawBlindCard(&card);
			replay_me = 1;
			printf("a\n");
		}
		else if (player == 0 && replay_me == 1) {
			me = drawBlindCard(&card);
			replay_me = 0;
			player = 1;
			printf("b\n");
		}
		else if (player == 1 && replay_opp == 0) {
			opponent = getMove(&opp_move, &replay_opp);
			printf("c\n");
		}
		else if (player == 1 && replay_opp == 1) {
			opponent = getMove(&opp_move, &replay_opp);
			player = 0;
			printf("d\n");
		}
		printMap();
	}
	
	if (opponent != 0) {
		printf("Bot lost\n");
	}
	if (me != 0) {
		printf("Maeru lost\n");
	}
	
	
	free(gameName);
	
	closeConnection();
	
	return 0;
}


