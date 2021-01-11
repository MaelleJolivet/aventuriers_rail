

all: jeu

jeu: clientAPI.o TicketToRideAPI.o connexion.o createDisplayGame.o playSmart.o
	gcc -o jeu clientAPI.o TicketToRideAPI.o connexion.o createDisplayGame.o playSmart.o

clientAPI.o: clientAPI.c clientAPI.h
	gcc -c clientAPI.c -Wall

TicketToRideAPI.o: TicketToRideAPI.c TicketToRideAPI.h clientAPI.h
	gcc -c TicketToRideAPI.c -Wall

connexion.o: clientAPI.h TicketToRideAPI.h connexion.c connexion.h
	gcc -c connexion.c -Wall

createDisplayGame.o: clientAPI.h TicketToRideAPI.h createDisplayGame.c connexion.h
	gcc -c createDisplayGame.c -Wall

playSmart.o: clientAPI.h TicketToRideAPI.h playSmart.c connexion.h
	gcc -c playSmart.c -Wall

clean: 
	rm clientAPI.o TicketToRideAPI.o connexion.o createDisplayGame.o playSmart.o

.PHONY: all clean
