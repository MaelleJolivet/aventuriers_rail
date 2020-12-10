

all: jeu

jeu: clientAPI.o TicketToRideAPI.o connexion.o
	gcc -o clientAPI.o TicketToRideAPI.o connexion.o

clientAPI.o: clientAPI.c clientAPI.h
	gcc -c clientAPI.c -Wall

TicketToRide.o: clientAPI.h TicketToRideAPI.c TicketToRideAPI.h
	gcc -c TicketToRideAPI.c -Wall

connexion.o: clientAPI.h TicketToRideAPI.h connexion.c
	gcc -c connexion.c -Wall

clean: 
	rm clientAPI.o TicketToRideAPI.o connexion.o

.PHONY: all clean
