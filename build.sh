#!/bin/bash
gcc `pkg-config --cflags gtk+-3.0` -o MAFIA_client mafia_client.c Mafia.c gtk.c `pkg-config --libs gtk+-3.0`
gcc `pkg-config --cflags gtk+-3.0` -o MAFIA_server mafia_server.c `pkg-config --libs gtk+-3.0`