#ifndef GTK_GRAFIC_H
#define GTK_GRAFIC_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GtkStruct {
    gchar name[64];
    GtkApplication *app;
    GtkWidget *window;
    GtkEntryBuffer *buffer, *buff;
    GtkTextBuffer *text;
    GtkWidget *button[6], *bsend;
    GtkWidget *label[6];
    GtkWidget *image;
};

static void activate (GtkApplication *, gpointer);
static void menu (GtkWidget *, gpointer, struct GtkStruct *); 
static void start_game (GtkWidget *, gpointer, struct GtkStruct *);
static void get_laws (GtkWidget *, gpointer, struct GtkStruct *);
static void end_game (struct GtkStruct *, char *);
static void vote (GtkWidget *, gpointer, int);
static void chat (GtkWidget *, gpointer, GtkEntryBuffer *);

#endif
