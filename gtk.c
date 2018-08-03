#include "gtk.h"

void vote (GtkWidget *widget, gpointer data, char *number) {
	if (send_msg(MSGTYPE_VOTE, number) == -1) { // Отправка голоса
		perror("send_vote");
		return -3;
	}
}

void chat (GtkWidget *widget, gpointer data, GtkEntryBuffer *buffer) {
	char *str = gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (buffer));
	if (send_msg(MSGTYPE_CHAT, str) == -1) { // Отправка сообщения в чат
		perror("send_vote");
		return -3;
	}
	gtk_entry_buffer_set_text (GTK_ENTRY_BUFFER (buffer), "", -1);
}

void stub (GtkWidget *widget, gpointer data) {}

void start_game (GtkWidget *widget, gpointer data, struct GtkStruct *arg) {
    int i, n = 6;
    int num[n];
    char name[32];
    guint padding;
    GtkWidget *window, *image[n + 1], *label[n];
    GtkEntryBuffer *buffer;
    GtkTextBuffer *tbuffer;
    GtkPackType child;
    GtkWidget *textarea;
    GtkWidget *entry;
    GtkWidget *button[n], *bsend;
    GtkWidget *boxh[3], *boxv[3], *boxb[n], *boxscroll;

    /*Создание контейнеров, в которых будут располагаться виджеты*/
    boxh[0] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 25);
    boxh[1] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 25);
    boxh[2] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    boxv[0] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 50);
    boxv[1] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    boxv[2] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    boxscroll = gtk_scrolled_window_new (NULL, NULL);

    /*Создание нового окна с копирование положения старого*/
    gint x, y;
    window = gtk_application_window_new ((arg)->app);
    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_window_get_position (GTK_WINDOW ((arg)->window), &x, &y);
    gtk_window_move (GTK_WINDOW (window), x - 200, y);
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);

    /*Создание поля ввода и вывод*/
    textarea = gtk_text_view_new ();
    tbuffer = gtk_text_buffer_new (NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (textarea), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textarea), FALSE);
    gtk_text_view_set_overwrite (GTK_TEXT_VIEW (textarea), FALSE);
    buffer = gtk_entry_buffer_new ("", -1);
    entry = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (buffer));
    gtk_text_view_set_buffer (GTK_TEXT_VIEW (textarea), tbuffer);
    image[6] = gtk_image_new_from_file ("./image/mafia.png");

    /*Создание кнопок для выбора пользователя и добавление их в горизонтальный контейнер*/
    bsend = gtk_button_new_with_label ("Отправить");
    g_signal_connect (bsend, "clicked", G_CALLBACK (chat), buffer);
    for (i = 0; i < n; i++) {
	sprintf (name, "Пользователь %d", i);
	label[i] = gtk_label_new (name);
	button[i] = gtk_button_new ();
	boxb[i] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (boxh[0]), boxb[i]);
	gtk_container_add (GTK_CONTAINER (boxb[i]), button[i]);
	gtk_container_add (GTK_CONTAINER (boxb[i]), label[i]);
	image[i] = gtk_image_new_from_file ("./image/live.png");
	gtk_button_set_image (GTK_BUTTON (button[i]), image[i]);
	num[i] = i;
	g_signal_connect (button[i], "clicked", G_CALLBACK (vote), &num[i]);
    }

    /*Вложение контейнеров друг в друга*/
    gtk_container_add (GTK_CONTAINER (window), boxv[0]);
    gtk_container_add (GTK_CONTAINER (boxv[0]), boxh[0]);
    gtk_container_add (GTK_CONTAINER (boxv[0]), boxh[1]);
    gtk_container_add (GTK_CONTAINER (boxh[1]), boxv[1]);
    gtk_container_add (GTK_CONTAINER (boxh[1]), boxv[2]);
    gtk_container_add (GTK_CONTAINER (boxv[1]), boxscroll);
    gtk_container_add (GTK_CONTAINER (boxscroll), textarea);
    gtk_container_add (GTK_CONTAINER (boxv[1]), boxh[2]);
    gtk_container_add (GTK_CONTAINER (boxh[2]), entry);
    gtk_container_add (GTK_CONTAINER (boxh[2]), bsend);
    gtk_container_add (GTK_CONTAINER (boxv[2]), image[6]);


    /*Включение режима расширение и заполнения*/
    gtk_box_query_child_packing (GTK_BOX (boxv[0]), boxh[1], NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxv[0]), boxh[1], TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxh[1]), boxv[1], NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxh[1]), boxv[1], TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxv[1]), boxscroll, NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxv[1]), boxscroll, TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxh[2]), entry, NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxh[2]), entry, TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxh[2]), bsend, NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxh[2]), bsend, FALSE, FALSE, padding, child);

	/*Активация и вывод на экран виджетов, а так-же удаление старого окна*/
    gtk_widget_show_all (window);
    gtk_widget_destroy (GTK_WIDGET ((arg)->window));
    arg->window = window;
    arg->text = tbuffer;
    arg->image = image[6];
    arg->bsend = bsend;
    for (i = 0; i < n; i++) {
	arg->button[i] = button[i];
	arg->label[i] = label[i];
    }
    pthread_t ptr;
    pthread_create (&ptr, NULL, logic, arg);
}

void end_game (struct GtkStruct *arg, char *msg) {
    GtkWidget *window, *label, *button, *box;
    gint x, y;
    window = gtk_application_window_new ((arg)->app);
    gtk_window_set_title (GTK_WINDOW (window), "End");
    gtk_window_set_default_size (GTK_WINDOW (window), 100, 50);
    gtk_window_get_position (GTK_WINDOW ((arg)->window), &x, &y);
    gtk_window_move (GTK_WINDOW (window), x + 200, y + 200);
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);

    label = gtk_label_new (msg);
    button = gtk_button_new_with_label ("OK");
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);

    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_container_add (GTK_CONTAINER (box), label);
    gtk_container_add (GTK_CONTAINER (box), button);

    g_signal_connect (button, "clicked", G_CALLBACK (menu), arg);

    gtk_widget_show_all (window);
    gtk_widget_destroy (GTK_WIDGET ((arg)->window));
    arg->window = window;
}

void get_laws (GtkWidget *widget, gpointer data, struct GtkStruct *arg) {
    GtkWidget *window, *textarea, *box;
    GtkTextBuffer *buffer;
    GtkWidget *button;
    GtkPackType child;
    FILE *fd;
    guint padding;
    if ((fd = fopen("./rules.txt", "r")) == NULL) {
	printf ("Can not open file\n");
	return;
    }
    gint x, y;
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    window = gtk_application_window_new ((arg)->app);
    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 600);
    gtk_window_get_position (GTK_WINDOW ((arg)->window), &x, &y);
    gtk_window_move (GTK_WINDOW (window), x, y);
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);

    textarea = gtk_text_view_new ();
    buffer = gtk_text_buffer_new (NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (textarea), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textarea), FALSE);
    gtk_text_view_set_overwrite (GTK_TEXT_VIEW (textarea), FALSE);
    gtk_text_view_set_buffer (GTK_TEXT_VIEW (textarea), buffer);

    gchar text[256];
    while (!feof (fd)) {
	fgets (text, 256, fd);
	printf ("%s\n", text);
	if (!feof (fd))
	    gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (buffer), text, -1);
    }

    button = gtk_button_new_with_label ("Назад");
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_container_add (GTK_CONTAINER (box), textarea);
    gtk_container_add (GTK_CONTAINER (box), button);

    gtk_box_query_child_packing (GTK_BOX (box), textarea, NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (box), textarea, TRUE, TRUE, padding, child);

    g_signal_connect (button, "clicked", G_CALLBACK (menu), arg);

    gtk_widget_show_all (window);
    gtk_widget_destroy (GTK_WIDGET ((arg)->window));
    arg->window = window;
}

void menu (GtkWidget *widget, gpointer user_data, struct GtkStruct *arg) {
    int i;
    GtkWidget *window;
    GtkWidget *button[3];
    GtkWidget *button_box;
    GtkWidget *box;
    GtkWidget *label;
    if (arg->buffer != NULL) {
	gchar *str = gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (arg->buffer));
	strcpy (arg->name, str);
	arg->buffer = NULL;
    }
    window = gtk_application_window_new ((arg)->app);

    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 600);
    gtk_container_set_border_width (GTK_CONTAINER (window), 75);

    button_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 25);
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 200);
    label = gtk_label_new ("Меню");
    
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_container_add (GTK_CONTAINER (box), label);
    gtk_container_add (GTK_CONTAINER (box), button_box);

    button[0] = gtk_button_new_with_label ("Игра");
    button[1] = gtk_button_new_with_label ("Правила");
    button[2] = gtk_button_new_with_label ("Выход");

    g_signal_connect (button[0], "clicked", G_CALLBACK (start_game), arg);
    g_signal_connect (button[1], "clicked", G_CALLBACK (get_laws), arg);
    g_signal_connect (button[2], "clicked", G_CALLBACK (gtk_widget_destroy), window);

    for (i = 0; i < 3; i++)
	gtk_container_add (GTK_CONTAINER (button_box), button[i]);

    gtk_widget_show_all (window);
    gtk_widget_destroy (GTK_WIDGET (arg->window));
    arg->window = window;
}

void activate (GtkApplication *app, gpointer user_date) {
    GtkWidget *window, *button, *box, *entry;
    GtkEntryBuffer *buffer;
    struct GtkStruct *arg = malloc (sizeof (struct GtkStruct));
    
    arg->app = app;
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 100, 50);
    gtk_container_set_border_width (GTK_CONTAINER (window), 75);
    arg->window = window;
    
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);

    button = gtk_button_new_with_label ("Подтвердить");
    buffer = gtk_entry_buffer_new ("", -1);
    entry = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (buffer));
    arg->buffer = buffer;
    
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_container_add (GTK_CONTAINER (box), entry);
    gtk_container_add (GTK_CONTAINER (box), button);
    
    g_signal_connect (button, "clicked", G_CALLBACK (menu), arg);
    gtk_widget_show_all (window);
}


