#ifndef QUARK_NET_H_
#define QUARK_NET_H_
void net_init();
void http_init();
void socket_create(char *name, char *ip, int port);
#endif