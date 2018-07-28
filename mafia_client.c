#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: UDP_CLIENT [address] [port]\n");
		return -2;
	}
	int socket_fd;
	size_t len;
	struct sockaddr_in target;
	socklen_t target_size;

	char buf[16] = {0};

	socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_fd == -1) {
		perror("socket");
		return -1;
	}
	
	memset(&target, 0, sizeof(target));
	target.sin_family = AF_INET;
	target.sin_port = htons(atoi(argv[2]));
	target.sin_addr.s_addr = inet_addr(argv[1]);
	target_size = sizeof(target);
	len = sizeof(buf);
	// Здесь может быть ваше меню

	printf("Enter your name: "); //Поле ввода имени появляется после нажатия Play
	scanf("%s", buf);
	printf("Your name is %s\n", buf);


	if (sendto(socket_fd, buf, len, 0, (struct sockaddr*)&target, target_size) == -1) {
		perror("send");
		return -3;
	}

	// Окно "Waiting" и ожидание ответа от сервера
	printf("Waiting...\n");
	if (recvfrom(socket_fd, buf, len, 0, (struct sockaddr*)&target, &target_size) == -1) {
		perror("recv");
		return -3;
	}
	printf("The GAME HAS BEGUN!\n");


	close(socket_fd);
	return 0;
}


