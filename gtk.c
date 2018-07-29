#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

struct GtkStruct {
    GtkApplication *app;
    GtkWidget *window;
};

static void start_game (GtkWidget *widget, gpointer data, struct GtkStruct *arg) {
    int i, n = 6;
    char name[32];
    guint padding;
    GtkWidget *window, *image, *label;
    GtkEntryBuffer *buffer;
    GtkTextBuffer *tbuffer;
    GtkPackType child;
    GtkWidget *textarea;
    GtkWidget *entry;
    GtkWidget *button[n];
    GtkWidget *boxh[2], *boxv[3];

    /*Создание контейнеров, в которых будут располагаться виджеты*/
    boxh[0] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 25);
    boxh[1] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 50);
    boxv[0] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 50);
    boxv[1] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    boxv[2] = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

    /*Создание нового окна с копирование положения старого*/
    gint x, y;
    window = gtk_application_window_new ((arg)->app);
    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_window_get_position (GTK_WINDOW ((arg)->window), &x, &y);
    gtk_window_move (GTK_WINDOW (window), x - 200, y);
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);

    /*Создание кнопок для выбора пользователя и добавление их в горизонтальный контейнер*/
    for (i = 0; i < n; i++) {
	sprintf (name, "Пользователь %d", i);
	button[i] = gtk_button_new_with_label (name);
	gtk_container_add (GTK_CONTAINER (boxh[0]), button[i]);
    }

    /*Создание поля ввода и вывод*/
    textarea = gtk_text_view_new ();
    tbuffer = gtk_text_buffer_new (NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (textarea), 0);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textarea), 0);
    gtk_text_view_set_bottom_margin (GTK_TEXT_VIEW (textarea), 25);
    gchar str[] = "test";
    buffer = gtk_entry_buffer_new (str, -1);
    entry = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (buffer));
    gchar *text = gtk_entry_buffer_get_text (GTK_ENTRY_BUFFER (buffer));
    printf ("%s\n", text);
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (tbuffer), str, -1);
    gtk_text_view_set_buffer (GTK_TEXT_VIEW (textarea), tbuffer);

    image = gtk_image_new_from_file ("/home/server1/image/mafia.jpg");
    label = gtk_label_new ("00:00");

    /*Вложение контейнеров друг в друга*/
    gtk_container_add (GTK_CONTAINER (window), boxv[0]);
    gtk_container_add (GTK_CONTAINER (boxv[0]), boxh[0]);
    gtk_container_add (GTK_CONTAINER (boxv[0]), boxh[1]);
    gtk_container_add (GTK_CONTAINER (boxh[1]), boxv[1]);
    gtk_container_add (GTK_CONTAINER (boxh[1]), boxv[2]);
    gtk_container_add (GTK_CONTAINER (boxv[1]), textarea);
    gtk_container_add (GTK_CONTAINER (boxv[1]), entry);
    gtk_container_add (GTK_CONTAINER (boxv[2]), image);
    gtk_container_add (GTK_CONTAINER (boxv[2]), label);

    /*Включение режима расширение и заполнения*/
    gtk_box_query_child_packing (GTK_BOX (boxv[0]), boxh[1], NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxv[0]), boxh[1], TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxh[1]), boxv[1], NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxh[1]), boxv[1], TRUE, TRUE, padding, child);
    gtk_box_query_child_packing (GTK_BOX (boxv[1]), textarea, NULL, NULL, &padding, &child);
    gtk_box_set_child_packing (GTK_BOX (boxv[1]), textarea, TRUE, TRUE, padding, child);

    
    

    gtk_widget_show_all (window);
    gtk_widget_destroy (GTK_WIDGET ((arg)->window));
}

static void get_laws (GtkWidget *widget, gpointer data) {
    return;
}

static void activate (GtkApplication *app, gpointer user_data) {
    int i;
    GtkWidget *window;
    GtkWidget *button[3];
    GtkWidget *button_box;
    GtkWidget *box;
    GtkWidget *label;
    struct GtkStruct *arg = malloc (sizeof (struct GtkStruct));
    arg->app = app;

    window = gtk_application_window_new (app);
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


    arg->window = window;
    g_signal_connect (button[0], "clicked", G_CALLBACK (start_game), arg);
    g_signal_connect (button[1], "clicked", G_CALLBACK (get_laws), NULL);
    g_signal_connect (button[2], "clicked", G_CALLBACK (gtk_widget_destroy), window);

    for (i = 0; i < 3; i++)
	gtk_container_add (GTK_CONTAINER (button_box), button[i]);

    gtk_widget_show_all (window);
}

int main (int    argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
