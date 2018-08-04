#ifndef GTK_GRAFIC_H
#define GTK_GRAFIC_H

#include "Mafia.h"

void activate (GtkApplication *, gpointer);
void menu (GtkWidget *, gpointer, struct GtkStruct *);
void start_game (GtkWidget *, gpointer, struct GtkStruct *);
void get_laws (GtkWidget *, gpointer, struct GtkStruct *);
void end_game (struct GtkStruct *, char *);
void votes (GtkWidget *, gpointer, char *);
void chat (GtkWidget *, gpointer, GtkEntryBuffer *);
void stub (GtkWidget *, gpointer);

#endif
