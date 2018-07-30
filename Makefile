all:
	gcc -Wall -o -std=c99 MAFIA_client mafia_client.c
	gcc -Wall -o -std=c99 MAFIA_server mafia_server.c
clean:
	rm MAFIA_client MAFIA_server

