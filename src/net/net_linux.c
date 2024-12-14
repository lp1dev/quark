#include <arpa/inet.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
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
	struct protoent *protoent;
	int socketfd;

	protoent = getprotobyname("tcp");
	socketfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (socketfd == -1) {
        printf("Could not create TCP socket %s\n", name);
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
	printf("Sending data [%s] into socket %i\n", string, socket);
	return send(socket, string, strlen(string), 0);
}

int socket_close(int socket) {
	// close(socket);
}