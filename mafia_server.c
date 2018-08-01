#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "Mafia.h"

struct itimerval tval;

int chatflag;

void handler(int signo) {
	assert(signo == SIGALRM);
	chatflag = 0;
}

struct Client { 			// Структура, описывающая клиента
		char name[NICK_LEN];		// Имя клиента, которое он отправляет
		char role;
		int status;				// 0 - мертв, 1 - жив
		struct sockaddr_in target;
};

struct Room {
		uint8_t count;
		struct Client clients[6];
};

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: MAFIA_server [ip] [port]\n");
		return -1;
	}

	struct Room room;
	struct message *msg;

	int vote_results[PLAYER_NUMBER];
	int indexmax, votemax, daycount, mafiacount, civilcount;
	int on = 1, i;
	int sock_fd, a, end = 0;
	size_t len;

	struct Client *sender;

	char buf[260], number[3] = {PLAYER_NUMBER - 2,1,1}, tempmsg[256];
	len = sizeof(buf);
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in target, server;
	srand(time(NULL));
	socklen_t target_size = sizeof(target), server_size = sizeof(server);

	(void)signal(SIGALRM, handler);
//	char string[BUFSIZ];


	memset(&server, 0, server_size);
	server.sin_family = AF_INET;
//	server.sin_port = htons(3425);
//	server.sin_addr.s_addr = inet_addr("10.0.2.15");
//	inet_pton(AF_INET, address, &target.sin_addr);
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt-reuseaddr");
		return -4;
	}
	if (bind(sock_fd, (struct sockaddr *) &server, server_size) == -1) {
		perror("bind");
		return -3;
	}

	msg = buf;

	while (room.count < PLAYER_NUMBER)	{ // Цикл принятия 6 имён
		memset(buf, 0, len);
		memset(&target, 0, target_size);
		do { // Принимаются и игнорируются все сообщения, кроме типа MSGTYPE_NICKNAME (подключившиеся клиенты могут спамить нажатиями кнопок)
			recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size); // Получение имени
		} while (msg->type != MSGTYPE_NICKNAME);
		room.clients[room.count].status = 1;

		//TODO: Оптимизировать алгоритм раздачи ролей
		a = rand() % 3;
		while(1) { //необходим для того, чтобы все роли были разданы, то есть пока все элементы массива не будут равны 0
			if(number[a] > 0){
				number[a]--;
				break;
			}
			else
				a = rand() % 3;
		}

		room.clients[room.count].role = a;//присваиваем выбранную роль для игрока, присвание идет через передачуу номера роли (0 - Мирный житель, 1 - Комиссар, 2 - Мафия)
		strcpy(room.clients[room.count].name, msg->buf); // Копирование имени в структуру лобби
		memcpy(&(room.clients[room.count].target), &target, target_size); // Копирование информации о пришедшем подключении (ip + port)

		/*
		 * Отправка оповещения о количестве подключенных в чат всем подключившимся до этого
		*/
		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "%d/%d...\n", room.count, PLAYER_NUMBER);
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr *) &(room.clients[i].target), target_size);
		}

		memset(buf, 0, len);
		msg->type = MSGTYPE_NICKNAME;
		for (i = 0; i < room.count; ++i) { // В этом цикле отсылаются ники подключённых подключившемуся и ник подключившегося подключённым (долше всего отправка будет отправка на 6 подключении
			msg->buf[0] = room.count;
			sprintf(msg->buf + 1, "%s", room.clients[room.count].name);
			sendto(sock_fd, buf, len, 0, (struct sockaddr *) &(room.clients[i].target), target_size);
			msg->buf[0] = i;
			sprintf(msg->buf + 1, "%s", room.clients[i].name);
			sendto(sock_fd, buf, len, 0, (struct sockaddr *) &(room.clients[room.count].target), target_size);
		} //TODO: Создать в клиенте массив из шести (включая себя) структур struct players { char name[32]; char alive; };

		room.count++;
		printf("Client connection from %s:%d - %s\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port), buf);
	}

	printf("\n%d\n", room.count);

	for (i = 0; i < room.count; ++i) {
		memset(buf, 0, len);
		msg->type = MSGTYPE_STARTEND;
		msg->buf[0] = 0;
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		memset(buf, 0, len);
		msg->type = MSGTYPE_INFO; // Клиенту отправляется его номер и его роль
		msg->buf[0] = i;
		msg->buf[1] = room.clients[i].role;
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
	}

	daycount = 0;

	do {

		daycount++; // Счётчик дней увеличивается при наступлении нового утра
		/*
		 * Рассылка всем игрокам сообщение о наступлении дня
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_DAYSTATE;
		msg->buf[0] = 0;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}
		/*
		 * Рассылка всем игрокам сообщение в чат о дне и запуске таймера на диалог
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "[server]: День %d.\n[server]: У вас 2 минуты на разговоры.\n", daycount);
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Инициализация и запуск таймера на 2 минуты
		 * */
		chatflag = 1;
		memset(&tval, 0, sizeof(tval));
		timerclear(&tval.it_interval); /* нулевой интервал означает не сбрасывать таймер */
		timerclear(&tval.it_value);
		tval.it_value.tv_sec = 120;
		(void)setitimer(ITIMER_REAL, &tval, NULL);


		while (chatflag) {
			/*
			 * Приём сообщений от игроков только с меткой msgtype_chat
			 * */
			do {
				recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size);
			} while (msg->type != MSGTYPE_CHAT);

			/*
			 * Поиск отправителя в списке игроков по ip и port
			 * */
			strcpy(tempmsg, msg->buf);
			sender = NULL;
			for (i = 0; i < room.count; ++i) {
				if ((target.sin_addr.s_addr == room.clients[i].target.sin_addr.s_addr)
						|| (target.sin_port == room.clients[i].target.sin_port)) {
					sender = &(room.clients[i]);
				}
			}

			/*
			 * Если нашёл отправителя, то рассылает всем игрокам его ник и сообщение
			 * Если не нашёл, то выводит в лог о неопознанном сообщении
			 * */
			if (sender != NULL) {
				memset(buf, 0, len);
				msg->type = MSGTYPE_CHAT;
				sprintf(msg->buf, "%s: %s\n", sender->name, tempmsg);
				for (i = 0; i < room.count; ++i) {
					sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
				}
			} else {
				printf("Unrecognized message from %s:%d: %s\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port), tempmsg);
			}

		}

		/*
		 * Отправка всем игрокам сообщение о начале голосования (изменения времени суток)
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_DAYSTATE;
		msg->buf[0] = 1;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}
		/*
		 * Рассылка всем игрокам сообщение в чат о начале голосования и предложении выбрать жертву
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "[server]: Начало голосования. Выберите жертву.\n");
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Приём 6 сообщений от игроков с их выбором
		 * */
		memset(vote_results, 0, sizeof(vote_results));
		for (i = 0; i < room.count; ++i) {
			do {
				recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size);
			} while (msg->type != MSGTYPE_VOTE);
			vote_results[msg->buf[0]]++;
		}

		/*
		 * Опознание трупа (Алгоритм влоб, переделать)
		 * */
		indexmax = votemax = -1;
		for (i = 0; i < room.count; ++i) {
			if (vote_results[i] > votemax) {
				votemax = vote_results[i];
				indexmax = i;
			}
		}

		if (room.clients[indexmax].role == 2) {
			end = 1;
			break;
		}

		/*
		 * Отправка всем о результате голосования
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_VOTERESULT;
		msg->buf[0] = indexmax;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		room.clients[indexmax].status = 0;

		/*
		 * Отправка всем игрокам сообщение о начале голосования (изменения времени суток)
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_DAYSTATE;
		msg->buf[0] = 2;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Отправка всем игрокам сообщение в чат о наступлении ночи и начале голосования мафии
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "[server]: Наступила ночь. Мафия выбирает жертву.\n");
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Получение голоса мафии (обернуть в цикл по количеству мафий, если мафий будет больше 1)
		 * */
		memset(vote_results, 0, sizeof(vote_results));
		do {
			recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size);
		} while (msg->type != MSGTYPE_VOTE);
		vote_results[msg->buf[0]]++;

		/*
		 * Опознание трупа (Алгоритм влоб, переделать)
		 * */
		indexmax = votemax = -1;
		for (i = 0; i < room.count; ++i) {
			if (vote_results[i] > votemax) {
				votemax = vote_results[i];
				indexmax = i;
			}
		}

		/*
		 * Отправка всем игрокам сообщение о начале проверки комиссара
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_DAYSTATE;
		msg->buf[0] = 2;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Отправка всем игрокам сообщение в чат о изменении времени суток и о начале проверки комиссара
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "[server]: Ночь идет. Комиссар проверяет человека.\n");
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
		}

		/*
		 * Получение голоса комиссара и передача ему роли проверяемого
		 * (очень ненадёжно, так как принимается сообщение на веру, без проверки, нужно блокировать отправку на стороне клиента)
		 * ((хотя это всё равно небезопасно))
		 * */
		do {
			recvfrom(sock_fd, buf, len, 0, (struct sockaddr *)&target, &target_size);
		} while (msg->type != MSGTYPE_VOTE);

		memset(buf, 0, len);
		msg->type = MSGTYPE_CHAT;
		sprintf(msg->buf, "[server]: %s - %s\n", room.clients[msg->buf[0]].name, roles[room.clients[msg->buf[0]].role]);
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&target, target_size);

		/*
		 * Отправка всем о результате голосования мафии
		 * */
		memset(buf, 0, len);
		msg->type = MSGTYPE_VOTERESULT;
		msg->buf[0] = indexmax;
		for (i = 0; i < room.count; ++i) {
			sendto(sock_fd, buf, len, 0, (struct sockaddr *) &(room.clients[i].target), target_size);
		}

		/*
		 * Проверяет в конце дня состояние игры, если остался 1х1, то побеждает мафия
		 * */
		mafiacount = civilcount = 0;
		for (i = 0; i < room.count; ++i) {
			if (room.clients[i].status == 1) {
				if (room.clients[i].role == 2) {
					mafiacount++;
				} else {
					civilcount++;
				}
			}
		}
		if (mafiacount == civilcount) {
			end = 2;
		}

	} while (end == 0);

	/*
	 * Отправка всем игрокам сообщение, что игра закончилась, победили те
	 * */
	memset(buf, 0, len);
	msg->type = MSGTYPE_STARTEND;
	msg->buf[0] = end;
	for (i = 0; i < room.count; ++i) {
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(room.clients[i].target), target_size);
	}

	close(sock_fd);
	return 0;
}
