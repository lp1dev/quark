#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "tcp_debugger.h"
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

int socket_set_nonblocking(int socket, int nonblocking) {
    return sceNetSetsockopt(socket, 
		SCE_NET_SOL_SOCKET, 
		SCE_NET_SO_NBIO, 
		&nonblocking, sizeof(nonblocking));
}

int socket_set_timeout(int socket, int timeout) {
    return sceNetSetsockopt(socket,
                     SCE_NET_SOL_SOCKET, 
					 SCE_NET_SO_RCVTIMEO, &timeout, 4);
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

NetConfig *get_device_info() {
	NetConfig *device_config;
	SceNetCtlInfo info;
	int ret;

	device_config = malloc(sizeof(NetConfig));
	ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_NETMASK, &info);
	if (ret < 0) {
		debug("DEBUG: Error getting netmask.\n", NULL);
	}
	debug("info.netmask-> ", info.netmask);
	strncpy(device_config->netmask, info.netmask, 16);

	ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_ETHER_ADDR, &info);
	if (ret < 0) {
		debug("DEBUG: Error getting mac address.\n", NULL);
	}

	debug("info.ether_addr.data-> ", info.ether_addr.data);
	memcpy(device_config->ether_addr, info.ether_addr.data, 6);

	ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info);
	if (ret < 0) {
		debug("DEBUG: Error getting ip addr.\n", NULL);
	}

	debug("info.ip_address-> ", info.ip_address);
	strncpy(device_config->ip_addr, info.ip_address, 16);

	ret = sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_DEFAULT_ROUTE, &info);
	if (ret < 0) {
		debug("DEBUG: Error getting default route.\n", NULL);
	}

	debug("info.default_route-> ", info.default_route);
	strncpy(device_config->gateway, info.default_route, 16);


	ret = sceNetCtlInetGetState(&device_config->state);
	if (ret < 0) {
		debug("DEBUG: Error getting network state.\n", NULL);
	}

	return device_config;
}

