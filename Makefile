all:
	gcc -Wall -o MAFIA_client mafia_client.c -std=c99
	gcc -Wall -o MAFIA_server mafia_server.c -std=c99
clean:
	rm MAFIA_client MAFIA_server

