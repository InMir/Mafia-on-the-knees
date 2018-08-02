#ifndef MAFIA_GTK_STRUCT_H
#define MAFIA_GTK_STRUCT_H

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

#endif //MAFIA_GTK_STRUCT_H
