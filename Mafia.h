#ifndef MAFIA_MAFIA_H
#define MAFIA_MAFIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include "gtk_struct.h"

#define MSGTYPE_CHAT 0
#define MSGTYPE_VOTE 1
#define MSGTYPE_DAYSTATE 2 // 0 - день(чат), 1 - день(голосование), 2 - ночь(мафия), 3 - ночь(комиссара)
#define MSGTYPE_VOTERESULT 3
#define MSGTYPE_NICKNAME 4
#define MSGTYPE_INFO 5
#define MSGTYPE_STARTEND 6 // 0 - начало игры, 1 - конец игры (победа мирных), 2 - конец игры (победа мафии)
#define MSGTYPE_OTHER 7

#define PLAYER_NUMBER 6
#define NICK_LEN 32

struct message {
	int type;
	char buf[256];
};

int send_msg (int , char *);
int logic (struct GtkStruct *);

// enum Roles {Civillian, Commissar, Mafia} roles;

#endif //MAFIA_MAFIA_H
