#all:
#	sudo apt-get install libgtk-3-dev
#	chmod +x build.sh
#	./build.sh
#clean:
#	rm MAFIA_client MAFIA_server
OBJECTS=mafia_client.c Mafia.c gtk.c
OBJ=mafia_server.c 
all: $(OBJECTS) $(OBJ)
	sudo apt-get install libgtk-3-dev
	gcc `pkg-config --cflags gtk+-3.0` -o MAFIA_client $(OBJECTS) `pkg-config --libs gtk+-3.0` -std=c99
	gcc `pkg-config --cflags gtk+-3.0` -o MAFIA_server $(OBJ) `pkg-config --libs gtk+-3.0` -std=c99

clean:
	rm MAFIA_client MAFIA_server
