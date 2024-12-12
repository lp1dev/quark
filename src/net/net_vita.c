#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <malloc.h>
#include <stdio.h>
#include "net.h"

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

void http_init() {
	printf("Loading module SCE_SYSMODULE_HTTP\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);

	printf("Running sceHttpInit\n");
	// sceHttpInit(1*1024*1024);
}

void ssl_init() {
    // sceSslInit(1024 * 1024);
}

void socket_create(char *name, char *ip, int port) {
    struct SceNetSockaddrIn addr = { 0 };
    int socket;

	socket = sceNetSocket(name, SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_TCP);
    if (socket < 0) {
		printf("PASV: sceNetSocket errno ret 0x%08X\n", socket);
        // exit(1);
	}

    addr.sin_len = sizeof(addr);
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_port = sceNetHtons(port);
    sceNetInetPton(SCE_NET_AF_INET, ip, &addr.sin_addr);
    // Probably to move somewhere else after
    sceNetConnect(socket, (SceNetSockaddr*)&addr, sizeof(addr));
    sceNetSend(socket, "VITAHERE\n", 9, 0);
    sceNetSocketClose(socket);
}