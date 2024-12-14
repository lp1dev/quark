#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "net.h"

/* 

void net_init()

	Initialize network (not necessary on Linux)

*/
void net_init() {
}

void http_init() {
}

void ssl_init() {
}

int socket_create_tcp(char *name) {
	// struct protoent *protoent;
	int socketfd;

	socketfd = socket(AF_INET, SOCK_STREAM, 6);
    if (socketfd == -1) {
        // exit(-1);
    }
	return socketfd;
}

int socket_connect(int socket, char *ip, int port) {
	struct sockaddr_in address;
	
	address.sin_family = AF_INET;
    address.sin_port = htons(port);
	inet_aton(ip, (struct in_addr *) &(address.sin_addr.s_addr));
    return connect(socket, (struct sockaddr *) &address, sizeof(address));
}

int socket_send_str(int socket, char *string) {
	return send(socket, string, strlen(string), 0);
}

int socket_close(int socket) {
	close(socket);
}