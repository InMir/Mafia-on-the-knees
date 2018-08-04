all:
	sudo apt-get install libgtk-3-dev
	chmod +x build.sh
	./build.sh
clean:
	rm MAFIA_client MAFIA_server

