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
int socket_tcp_ping(int socket, char *ip, int port);
int socket_ping(char *host);

typedef struct {
    char ip_addr[16];
    char netmask[16];
    char gateway[16];
    unsigned char ether_addr[6];
    int state;
// States are based on SceNetCtlState { SCE_NETCTL_STATE_DISCONNECTED , SCE_NETCTL_STATE_CONNECTING , SCE_NETCTL_STATE_FINALIZING , SCE_NETCTL_STATE_CONNECTED }
} NetConfig;

NetConfig *get_device_info();


#endif