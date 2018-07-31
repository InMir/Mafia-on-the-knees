#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#define number_playrs 2

struct Client { 			// Структура, описывающая клиента
		char Name[16];		// Имя клиента, которое он отправляет
		char Role[16];
		int Status;//0-мертв 1-жив
		struct sockaddr_in target;
};

struct Lobby {
		uint8_t count;
		struct Client clients[6];
};
int main(int argc, char **argv) {
	//if (argc != 3) {
	//	printf("Usage: MAFIA_server [ip] [port]\n");
	//	return -1;
	//}

	int dop[number_playrs],j;
	int k,z,max; 
	struct Lobby lobby = {0};

	int sock_fd, ret,a, player_number,flag=1,number_votes=0,b,result_votes[6];
	size_t len;

	char buf[16] = {0},buf1[16] = {0},*Roles[3]={"Житель","Комиссар","Мафия"},number[3]={number_playrs-2,1,1};//,*address="10.0.2.15";
	len = sizeof(buf);
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int on = 1,i;
	struct sockaddr_in target, server;
	srand(time(NULL));
	socklen_t target_size = sizeof(target), server_size = sizeof(server);

	struct itimerval tval;
  	char string[BUFSIZ];
  	timerclear(&tval.it_interval); /* нулевой интервал означает не сбрасывать таймер */
 	timerclear(&tval.it_value);
  	tval.it_value.tv_sec = 3;
  	(void)signal(SIGALRM, handler);


	memset(&server, 0, server_size);
	server.sin_family = AF_INET;
	server.sin_port = htons(3425);
	//inet_pton(AF_INET, address, &target.sin_addr);
	server.sin_addr.s_addr = inet_addr("10.0.2.15");
	//server.sin_port = htons(atoi(argv[2]));
	//server.sin_addr.s_addr = inet_addr(argv[1]);

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt-reuseaddr");
		return -4;
	}
	if (bind(sock_fd, (struct sockaddr *) &server, server_size) == -1) {
		perror("bind");
		return -3;
	}
		for(i=0; i< number_playrs;i++){
			lobby.clients[i].Status=1;
		}
	while (lobby.count < number_playrs)	{ // Цикл принятия 6 имён
		memset(buf, 0, len);
		memset(&target, 0, target_size);
		recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size); // Получение имени
		a=rand()%3;
		while(1){//необходим для того, чтобы все роли были разданы, то есть пока все элементы массива не будут равны 0
			if(number[a]>0){
				number[a]--;
				break;
			}
			else
				a=rand()%3;
		}
		strcpy(lobby.clients[lobby.count].Role, Roles[a]);//присваиваем выбранную роль для игрока
		strcpy(lobby.clients[lobby.count].Name, buf); // Копирование имени в структуру лобби
		memcpy(&(lobby.clients[lobby.count].target), &target, target_size); // Копирование информации о пришедшем подключении (ip + port)
		lobby.count++;
		printf("Client connection from %s:%d - %s\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port), buf);
	}
	printf("\n%d\n",lobby.count);
	for (i = 0; i < lobby.count; ++i) { // Цикл отправки подключившимся клиентам их роль
		//sprintf(buf, "Welcome!",lobby.clients[lobby.count].Role);
		strcpy(buf, lobby.clients[i].Role);
		sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(lobby.clients[i].target), target_size);
	}
	while(1){
		printf("\nПросыпается город:\n");
		if(!flag){//если отправляем сообщение клиенту
			for(i=0;i<number_playrs;i++){
				if(lobby.clients[i].Status==0){
					printf("\nИгрок под номером %d убит\n",i);
					sprintf(buf,"%s","0");
					sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(lobby.clients[i].target), target_size);
				}
				sprintf(buf,"%s","ыва");
				sendto(sock_fd, buf, len, 0, (struct sockaddr*)&(lobby.clients[i].target), target_size);
			}
				if(strcmp(lobby.clients[b].Role,"Мафия")==0){
					printf("\nПобеда, мафия убита!!!\n");
					break;
				}
		}
		flag=0;
		printf("\nЧат:\n");
		while (1){ // Цикл принятия 6 имён
			memset(buf, 0, len);
			memset(&target, 0, target_size);
			printf("vote");
			recvfrom(sock_fd, buf, len, 0, (struct sockaddr *) &target, &target_size); // Получение имени
			//printf("vote1");
				if(strcmp(buf,"vote")==0){//прием голосов
					printf("vote");
					for(i=0;i<number_playrs;i++)
					{
						recvfrom(sock_fd, buf1, len, 0, (struct sockaddr *) &target, &target_size);
						result_votes[i] = atoi(buf1);
					}
					printf("Убит игрок под номером:");
					for(i=0;i<number_playrs;i++)
					{
						printf("%d\n", result_votes[i]);
					}
					printf("222323");		
					
					
				}
				else{
					printf("%s\n", buf);			
				}
		}
		printf("\nГород засыпает, просыпается мафия\n");
		printf("\nМафия убивает ирока под номером: ");
		scanf("%d", &a);
		lobby.clients[a].Status=0;
		printf("\nМафия  засыпает, просыпается комиссар\n");
		printf("\nКомиссар проверяет ирока под номером: ");
		scanf("%d", &b);
		printf("\nКомиссар засыпает:\n");
		// Запуск игры
	}
	close(sock_fd);
	/*
	не добавил правило: Поскольку ночью мафия промахнуться не может, 
	т.к. выборка идет только из мирных, когда в игре остается 1х1 мафия выигрывает
	*/
	return 0;
}
