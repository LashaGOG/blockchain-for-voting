#Déclarations de variables
#Le compilateur
CC = gcc
#Les options du compilateurs
CFLAGS = -g -Wall -lssl -lcrypto
#Liste des programme à créer
PROGRAMS = main

#Premiere regle: liste des programme à compiler
#Règle sans action, seulement avec des dépendances
all : $(PROGRAMS)

#Règle pour compiler le main
main : main.o primalite.o protocole_rsa.o keys.o generate_data.o rw_data.o gagnant.o blockchain.o processus_vote.o
	$(CC) -g -Wall -o $@ $^ -lssl -lcrypto

#Règle générique de compilation des .o à partir des .c
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

#Effacer les .o et les executables
#Pour executer cette regle il faut taper dans le terminal "make clean"
clean : 
	rm *.o $(PROGRAMS)
