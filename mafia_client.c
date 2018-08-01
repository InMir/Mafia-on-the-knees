#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Mafia.h"


struct sockaddr_in target;
socklen_t target_size;
int sock_fd;//	struct itimerval tval;
//  	char string[BUFSIZ];
//  	timerclear(&tval.it_interval); /* нулевой интервал означает не сбрасывать таймер */
// 	timerclear(&tval.it_value);
//  	tval.it_value.tv_sec = 3;
//  	(void)signal(SIGALRM, handler);

int daytime = 0; //0 - ночь, 1 - день
int end = 0; // 0 - игра идёт, 1 - конец игры
char buf[260], nickname[NICK_LEN];
char alive[6]; // Массив показателей жизненного состояния жителей (1 - жив, 0 - мертв)
char my_role, my_number;

//void handler(int signo) {
//	char msg[] = "\n\nГолосование, введите номер игрока, которого хотите убить: ";
//	assert(signo == SIGALRM);
//  write(2, msg, sizeof(msg) - 1);
//  flag1 = 1;
//  //exit(1);
//}


int send_msg(int msgtype, char *str) {
	if ((str == NULL) || (strlen(str) > 256)) {
		return -1;
	}
	char localbuf[260];
	struct message *msg = localbuf;
	msg->type = msgtype;
	memcpy(msg->buf, str, 256);
	return sendto(sock_fd, localbuf, sizeof(localbuf), 0, (struct sockaddr*)&target, target_size);
}

int main(int argc, char *argv) {
	if (argc != 3) {
		printf("Usage: ./MAFIA_client [ip] [port]\n");
		return 0;
	}
	int flag = 1, flag2 = 1, vote;
	size_t len;

	for (int i = 0; i < 6; ++i) {		// Заполенеие массива alive единицами,
		alive[i] = 1;									// так как изначально все жители живы
	}

	struct message *msg;

	 //,*address="10.0.2.15";

	//таймер
/*
	struct itimerval tval;
	char string[BUFSIZ];
  timerclear(&tval.it_interval); //нулевой интервал означает не сбрасывать таймер

  timerclear(&tval.it_value);
  tval.it_value.tv_sec = 10; //тайм-аут 10 секунд

  (void)signal(SIGALRM, handler);
*/

  	//сокет
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd == -1) {
		perror("socket");
		return -1;
	}
	memset(&target, 0, sizeof(target));
	target.sin_family = AF_INET;
  target.sin_port = htons(atoi(argv[2]));
	target.sin_addr.s_addr = inet_addr(argv[1]);
	target_size = sizeof(target);
	len = sizeof(buf);



	printf("Enter your name: "); //Поле ввода имени появляется при запуске перед меню
	scanf("%s", nickname);
	printf("Your name is %s\n", nickname);
	// Здесь может быть ваше меню


	if (send_msg(MSGTYPE_NICKNAME, nickname) == -1) { // Отправка ника по нажатию кнопки Играть
		perror("send_nick");
		return -3;
	}


	msg = buf;
	printf("The GAME HAS BEGUN!\n");
	do {
		recvfrom(sock_fd, buf, sizeof(buf), 0, NULL, NULL);
		switch (msg->type) {
			case MSGTYPE_DAYSTATE:
				daytime = msg->buf[0]; //от сервера приходит сообщение, что изменось время суток. Чат ночью блокируется
				break;
			case MSGTYPE_CHAT:
				//gtk_insert в окно с чатом
				break;
			case MSGTYPE_VOTERESULT:
				alive[msg->buf[0]] = 0; // Блокировать отправку всех send'ов, если умеры ты, изменить иконку если умер кто-то другой
				break;
			case MSGTYPE_INFO: // Получение данных о себе
				my_number = msg->buf[0];
				my_role = msg->buf[1];
				break;
			case MSGTYPE_STARTEND:
				end = msg->buf[0]; // Конец игры, объявляются победители
				if (end == 1) {
					 /*
					  * Конец игры. В чат или отдельным окном вывести сообщение о победителе, передаваемую сервером в msg->buf[1]
					  * например if (msg->buf[1] == 2) {
					  * 	printf("Победа МАФИИ!\n");
					  * } else {
					  * 	printf("Победа МИРНЫХ!\n");
					  * }
					  * */

				}
				break;
			case MSGTYPE_NICKNAME:

			default:
				break;

		}

	} while (!end);

	close(sock_fd);
	return 0;
}
