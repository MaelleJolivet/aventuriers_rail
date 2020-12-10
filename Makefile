

all: jeu

jeu: clientAPI.o TicketToRideAPI.o connexion.o
	gcc -o jeu clientAPI.o TicketToRideAPI.o connexion.o

clientAPI.o: clientAPI.c clientAPI.h
	gcc -c clientAPI.c -Wall

TicketToRideAPI.o: TicketToRideAPI.c TicketToRideAPI.h clientAPI.h
	gcc -c TicketToRideAPI.c -Wall

connexion.o: clientAPI.h TicketToRideAPI.h connexion.c connexion.h
	gcc -c connexion.c -Wall

clean: 
	rm clientAPI.o TicketToRideAPI.o connexion.o

.PHONY: all clean
