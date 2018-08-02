#!/bin/bash
gcc `pkg-config --cflags gtk+-3.0` -o MAFIA_client main.c Mafia.c gtk.c `pkg-config --libs gtk+-3.0`
