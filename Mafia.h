#ifndef MAFIA_MAFIA_H
#define MAFIA_MAFIA_H


#define MSGTYPE_CHAT 0
#define MSGTYPE_VOTE 1
#define MSGTYPE_DAYNIGHT 2 //0 - ночь, 1 - день
#define MSGTYPE_VOTERESULT 3
#define MSGTYPE_NICKNAME 4
#define MSGTYPE_INFO 5
#define MSGTYPE_STARTEND 6 //0 - начало игры, 1 - конец игры
#define MSGTYPE_OTHER 7

#define PLAYER_NUMBER 6

struct message {
		int type;
		char buf[256];
};

enum Roles {Civillian, Commissar, Mafia} roles;

#endif //MAFIA_MAFIA_H
