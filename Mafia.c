
#include "gtk.h"

int sock_fd;
struct sockaddr_in target;
socklen_t target_size;

int send_msg (int msgtype, char *str) {
	if ((str == NULL) || (strlen(str) > 256)) {
		return -1;
	}
	char localbuf[260];
	struct message *msg = localbuf;
	msg->type = msgtype;
	memset (localbuf, 0, 260);
	msg->type = msgtype;
	memcpy (msg->buf, str, 256);
	return sendto(sock_fd, localbuf, sizeof(localbuf), 0, (struct sockaddr*)&target, target_size);
}

void *logic (void *art) {
	struct GtkStruct *arg = art;
	GtkEntryBuffer *buffer;
	char *imagepath[3] = {"./image/civil.png", "./image/comm.png", "./image/mafia.png"};
	int daytime = 0; //0 - ночь, 1 - день
	int end = 0; // 0 - игра идёт, 1 - конец игры
	char buf[260], nickname[NICK_LEN];
	char alive[6], num[6]; // Массив показателей жизненного состояния жителей (1 - жив, 0 - мертв)
	char my_role, my_number;
	int flag = 1, flag2 = 1, vote;
	size_t len;
	for (int i = 0; i < 6; ++i) {		// Заполенеие массива alive единицами,
		alive[i] = 1;			// так как изначально все жители живы
		num[i] = i;
	}
	struct message *msg;
	buffer = arg->buff;
	printf ("1%p\n", buffer);

	//сокет
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd == -1) {
		perror("socket");
		return -1;
	}
	memset(&target, 0, sizeof(target));
	target.sin_family = AF_INET;
	target.sin_port = htons(7777);
	target.sin_addr.s_addr = inet_addr("192.168.1.9");
	target_size = sizeof(target);
	len = sizeof(buf);

	printf("Your name is %s\n", arg->name);

	if (send_msg(MSGTYPE_NICKNAME, arg->name) == -1) { // Отправка ника по нажатию кнопки Играть
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
				if (alive[my_number] == 1)
					switch (daytime) {
						case 0:
							//g_signal_connect (arg->bsend, "clicked", G_CALLBACK (chat), buffer);
							for (int i = 0; i < 6; i++) {
								g_signal_handler_disconnect (arg->button[i], arg->id[i]);
								//g_signal_connect (arg->button[i], "clicked", G_CALLBACK (stub), NULL);
							}
							break;
						case 1:
							//g_signal_connect (arg->bsend, "clicked", G_CALLBACK (stub), NULL);
							for (int i = 0; i < 6; i++)
								if ((alive[i] == 1)) {
									//g_signal_handler_disconnect (arg->button[i], arg->id[i]);
									arg->id[i] = g_signal_connect (arg->button[i], "clicked", G_CALLBACK (votes), &num[i]);
								}
							break;
						case 2:
							if (my_role != 2)
								for (int i = 0; i < 6; i++) {
									g_signal_handler_disconnect (arg->button[i], arg->id[i]);
									//g_signal_connect (arg->button[i], "clicked", G_CALLBACK (stub), NULL);
								}
							break;
						case 3:
							if (my_role != 1)
								for (int i = 0; i < 6; i++) {
									g_signal_handler_disconnect (arg->button[i], arg->id[i]);
									//g_signal_connect (arg->button[i], "clicked", G_CALLBACK (stub), NULL);
								}
							else
								for (int i = 0; i < 6; i++)
									if ((alive[i] == 1)) {
										//g_signal_handler_disconnect (arg->button[i], arg->id[i]);
										arg->id[i] = g_signal_connect (arg->button[i], "clicked", G_CALLBACK (votes), &num[i]);
									}
							break;
					}
				break;
			case MSGTYPE_CHAT:
				gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (arg->text), msg->buf, -1);
				break;
			case MSGTYPE_VOTERESULT:
				alive[msg->buf[0]] = 0; // Блокировать отправку всех send'ов, если умеры ты, изменить иконку если умер кто-то другой
				if (msg->buf[0] == my_number)
					for (int i = 0; i < 6; i++)
						g_signal_handler_disconnect (arg->button[i], arg->id[i]);
					g_signal_handler_disconnect (arg->bsend, arg->id[6]);
				GtkWidget *image = gtk_image_new_from_file ("./image/dead.png");
				gtk_button_set_image (GTK_BUTTON (arg->button[msg->buf[0]]), image);
				break;
			case MSGTYPE_INFO: // Получение данных о себе
				my_number = msg->buf[0];
				my_role = msg->buf[1];
				gtk_label_set_text (GTK_LABEL (arg->label[my_number]), arg->name);
				gtk_image_set_from_file (GTK_IMAGE (arg->image), imagepath[my_role]);
				break;
			case MSGTYPE_STARTEND:
				end = msg->buf[0]; // Конец игры, объявляются победители
				if (end == 1) {
					//Конец игры. В чат или отдельным окном вывести сообщение о победителе, передаваемую сервером в msg->buf[1]
					if (msg->buf[1] == 2) {
						char str[] = "Победа МАФИИ!";
						end_game (arg, str);
						return 0;
					} else {
					 	char str[] = "Победа МИРНЫХ!";
						end_game (arg, str);
						return 0;
					}
				}
				break;
			case MSGTYPE_NICKNAME:
				printf ("%s\n", msg->buf + 1);
				gtk_label_set_text (GTK_LABEL (arg->label[msg->buf[0]]), msg->buf + 1);
				break;
			default:
				break;
		}

	} while (end == 0);

	close(sock_fd);
	return 0;
}
