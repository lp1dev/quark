#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "net.h"

/* 

void net_init()

	Initialize network for the PS Vita

*/
void net_init() {
	printf("Loading module SCE_SYSMODULE_NET\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

	printf("Running sceNetInit\n");
	SceNetInitParam netInitParam;
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	printf("Running sceNetCtlInit\n");
	sceNetCtlInit();
}

/* 

void net_init()

	Initialize http libs for the PS Vita

*/
void http_init() {
	printf("Loading module SCE_SYSMODULE_HTTP\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);

	printf("Running sceHttpInit\n");
	// sceHttpInit(1*1024*1024);
}

/* 

void ssl_init()

	Initialize ssl for the PS Vita

*/
void ssl_init() {
    // sceSslInit(1024 * 1024);
}

/* 

int socket_create_tcp(char *name);

	Create TCP Socket.

*/
int socket_create_tcp(char *name) {
    int socket;

	socket = sceNetSocket(name, SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_TCP);
    if (socket < 0) {
		printf("PASV: sceNetSocket errno ret 0x%08X\n", socket);
        // exit(1);
	}
	return socket;
}

/* 

void socket_connect(int socket, char *ip, int port);

	Connect a socket to an IP and port.

*/
int socket_connect(int socket, char *ip, int port) {
    struct SceNetSockaddrIn addr = { 0 };

    addr.sin_len = sizeof(addr);
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_port = sceNetHtons(port);
    sceNetInetPton(SCE_NET_AF_INET, ip, &addr.sin_addr);
    // Probably to move somewhere else after
    return sceNetConnect(socket, (SceNetSockaddr*)&addr, sizeof(addr));
}

/* 

int socket_send_str(int socket, char *string);

	Send a string through a socket.

*/
int socket_send_str(int socket, char *string) {
	int flags;
	int string_size;

	flags = 0;
	string_size = strlen(string);
	return sceNetSend(socket, string, string_size, flags);
}

/* 

int socket_close(int socket);

	Close a socket.

*/
int socket_close(int socket) {
	sceNetSocketClose(socket);
}