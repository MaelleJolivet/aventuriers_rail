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
 
 => pick 1 to 3 objectives (draw + chose) : start with 1
 
 => find which routes we HAVE to take to complete the objectives :
	-> shorter path ?
	-> higher points ?
	-> fastest (less rounds) ?

 THEN at each round
 => check if we have completed (an) objective(s)
	-> if yes :
		- if the game is far from over, take a new one
 => if we have enough cards to take a route we want, we take it
	-> if we can take several routes, choose the highest in points ?
 => else, we pick cards to help us take the routes we want
	-> faceUp if we are interested
	-> blindCard else
 
 
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
				distances[j] = distances[u] + routes[u][j].cars;
				previous[j] = u;
			}
			//i own the route => distance == 0
			else if (routes[u][j].free == game->me) {
				distances[j] = 0;
				previous[j] = u; //bof mais à voir plus tard
			}
		}
		u = distanceMini(distances, visited);
	}
	
}


int storeSourcetoDest (int source, int destination, int previous[36], int sourceToDest[78], t_route routes[36][36]) {
	int city = destination;
	int i = 0;
	sourceToDest[i] = city;

	while (city != source) {
		city = previous[city];
		sourceToDest[i+1] = city;
		i++;
	}
	//sourceToDest ex: [6,5,7,8,9] meaning we need the routes (6,5) (5,7) (7,8) (8,9)
	return i;
}
