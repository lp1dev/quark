#ifndef QUARK_NET_H_
#define QUARK_NET_H_

void net_init();
void http_init();
void ssl_init();
int socket_create_tcp(char *name);
int socket_set_timeout(int socket, int timeout);
int socket_set_nonblocking(int socket, int nonblocking);
int socket_send_str(int socket, char *string);
int socket_close(int socket);
int socket_connect(int socket, char *ip, int port);

#endif