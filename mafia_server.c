#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


struct Client { 			// Структура, описывающая клиента
		char Name[16];		// Имя клиента, которое он отправляет
		struct sockaddr_in target;
};

struct Lobby {
		uint8_t count;
		struct Client clients[6];
};


int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: MAFIA_server [ip] [port]\n");
		return -1;
	}

	struct Lobby lobby = {0};

	int sock_fd, ret;
	size_t len;

	char buf[16] = {0};
	len = sizeof(buf);
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int on = 1;
	
	struct sockaddr_in target, server;
	socklen_t target_size = sizeof(target), server_size = sizeof(server);
	memset(&server, 0, server_size);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt-reuseaddr");
		return -4;
	}
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0) {
		perror("setsockopt-reuseport");
		return -4;
	}


	if (bind(sock_fd, (struct sockaddr *) &server, server_size) == -1) {
		perror("bind");
		return -3;
	}

	while (lobby.count < 6)	{ // Цикл принятия 6 имён
		memset(buf, 0, len);
		memset(&target, 0, target_size);
		recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size); // Получение имени
		strcpy(lobby.clients[lobby.count].Name, buf); // Копирование имени в структуру лобби
		memcpy(&(lobby.clients[lobby.count].target), &target, target_size); // Копирование информации о пришедшем подключении (ip + port)
		lobby.count++;
		printf("Client connection from %s:%d - %s\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port), buf);
	}
	for (int i = 0; i < lobby.count; ++i) { // Цикл отправки подключившимся клиентам приветствия
		sprintf(buf, "Welcome!");
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(lobby.clients[i].target), target_size);
	}

	// Запуск игры

	close(sock_fd);

	return 0;
}



