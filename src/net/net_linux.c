#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "net.h"
#include "tcp_debugger.h"
#define TCP_USER_TIMEOUT 18 

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

	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// Setting sockets to
	// nonblock makes TCP scanning much faster.
	// Less reliable for other operations, but
	// Necessary here
    if (socketfd == -1) {
		// Cannot call debug here, we'd be in an infinite loop!
		printf("Could not create new socket.\n");
        exit(-1);
    }
	//
	return socketfd;
}

int socket_set_timeout(int sock, int timeout) {
	// Timeout should be in milliseconds
	int ret;
	ret = setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof (timeout));
}

int socket_set_nonblocking(int sock, int nonblocking) {
	// At the moment, we set nonblocking when initializing the socket
	// int ret;
	// ret = setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT, (char*) &timeout, sizeof (timeout));
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

int socket_ping(char *host) {
	// TODO implement
	return -1;
}

int socket_tcp_ping(int socket, char *ip, int port) {
}

NetConfig *get_device_info(){
	NetConfig *config;
	//TODO
	config = malloc(sizeof(NetConfig));
	return config;
}