#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
int flag1 = 0;
char buf[16] = {0},buf1[16] = {0};
 void handler(int signo)
 {
  char msg[] = "\n\nГолосование, введите номер игрока, которого хотите убить: ";
  assert(signo == SIGALRM);
  write(2, msg, sizeof(msg) - 1);
  flag1=1;
  //exit(1);
 }

int main()
{


	int socket_fd,flag=1,flag2=1,vote;
	size_t len;
	struct sockaddr_in target;
	socklen_t target_size;

	 //,*address="10.0.2.15";

	//таймер
	struct itimerval tval;
  	char string[BUFSIZ];
  	timerclear(&tval.it_interval); /* нулевой интервал означает не сбрасывать таймер */
  	timerclear(&tval.it_value);
  	tval.it_value.tv_sec = 10; /* тайм-аут 10 секунд */
  	(void)signal(SIGALRM, handler);

  	//сокет
	socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_fd == -1) {
		perror("socket");
		return -1;
	}
	memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(3425);
    target.sin_addr.s_addr = inet_addr("10.0.2.15");
	target_size = sizeof(target);
	len = sizeof(buf);
	// Здесь может быть ваше меню
	printf("Enter your name: "); //Поле ввода имени появляется после нажатия Play
	scanf("%s", buf);
	printf("Your name is %s\n", buf);


	if (sendto(socket_fd, buf, len, 0, (struct sockaddr*)&target, target_size) == -1) {
		perror("send1");
		return -3;
	}
		// Окно "Waiting" и ожидание ответа от сервера
	printf("Waiting...\n");
	if (recvfrom(socket_fd, buf, len, 0, (struct sockaddr*)&target, &target_size) == -1) {
		perror("recv");
		return -3;
	}
	printf("You is %s\n",buf);
	printf("The GAME HAS BEGUN!\n");
	printf("Enter message: ");
	while(1){ 
		
		if(!flag){//если получаем сообщения
			recvfrom(socket_fd, buf, len, 0, (struct sockaddr*)&target, &target_size);
			if(strcmp(buf,"0")==0){//червер отправляет "0", когда мафия убивает игрока
				printf("Ты проиграл!!!");
				close(socket_fd);
				return 0;
			}
			else
				printf("Ты жив!!!");
		}
		flag=0;
		(void)setitimer(ITIMER_REAL, &tval, NULL);//включаем таймер
		/*
			цикл, в котором игрок отправляет сообщения в чат, попробовал через флаги проконтролировать таймер, который при истечение 
			генерирует сигнал SIGALRM, но не получается прервать выполнение цикла, короче надо тут что-нибудь придумать с таймером, 
			либо сделать другую альтернативу таймера
		*/
		while(1){
			if(!flag1)
			{
				scanf("%s", buf);
				if(!flag1){
					if(sendto(socket_fd, buf, len, 0, (struct sockaddr*)&target, target_size) == -1) {
						perror("send2");	
						return -3;
					}
				}
				else
					break;
				//puts(buf);
			}
			else
				break;
			
		}
		if(flag1){//если время на переписку между клиентами закончилось, то игроки начинают голосовать и посылает серверу пакет vote
			sprintf(buf1,"%s","vote");	
			if(sendto(socket_fd, buf1, len, 0, (struct sockaddr*)&target, target_size) == -1) {
				perror("send2");	
				return -3;
			}
			//flag2=0;
			flag1=0;
			puts(buf1);
		}
		//scanf("%s", buf);
			if(sendto(socket_fd, buf, len, 0, (struct sockaddr*)&target, target_size) == -1) {
						perror("send2");	
						return -3;
					}


	}

	close(socket_fd);
    return 0;
}
