all:
	gcc -Wall -o UDP_CLIENT udp_client.c
	gcc -Wall -o UDP_SERVER udp_server.c
clean:
	rm UDP_CLIENT UDP_SERVER

