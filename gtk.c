#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

struct GtkStruct {
    GtkApplication *app;
    GtkWidget *window;
};

static void start_game (GtkWidget *widget, gpointer data, struct GtkStruct *arg) {
    int n = 6;
    GtkWidget *window;
    GtkWidget *buffer, *entry;
    GtkWidget *button[n];
    GtkWidget *boxh, *boxv;

    boxh = gtk_box_new (

    gint x, y;

    window = gtk_application_window_new ((arg)->app);
    gtk_window_set_title (GTK_WINDOW (window), "Mafia");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_window_get_position (GTK_WINDOW ((arg)->window), &x, &y);
    gtk_window_move (GTK_WINDOW (window), x - 200, y);

    

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
