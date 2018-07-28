all:
	gcc -Wall -o MAFIA_client mafia_client.c
	gcc -Wall -o MAFIA_server mafia_server.c
clean:
	rm MAFIA_client MAFIA_server

