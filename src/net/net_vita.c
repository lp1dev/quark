// #include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
// #include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "tcp_debugger.h"
#include "vita_structs.h"
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
	// struct timeval tv;

	// tv.tv_sec = timeout / 1000;
    // tv.tv_usec = (timeout % 1000) * 1000;
	// result = sceNetSelect(sock + 1, &writefds, NULL, NULL, &tv);
	// // It seems that we have no actual TCP timeout option on net for the PS Vita 
	// int flag = 1;
	// sceNetSetsockopt(socket, SCE_NET_IPPROTO_TCP, SCE_NET_TCP_NODELAY, (char *)&flag, sizeof(int));
	// sceNetSetsockopt(socket, SCE_NET_SOL_SOCKET, SCE_NET_SO_KEEPALIVE, (char *)&timeout, sizeof(int));

    // return sceNetSetsockopt(socket,
    //                  SCE_NET_SOL_SOCKET, 
	// 				 SCE_NET_SO_RCVTIMEO, &timeout, sizeof(timeout));
}

uint16_t in_cksum(uint16_t *ptr, int32_t nbytes){
	uint32_t sum;
	sum = 0;
	while (nbytes > 1){
		sum += *(ptr++);
		nbytes -= 2;
	}

	if (nbytes > 0)
		sum += *(char*)ptr;

	while (sum>>16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (~sum);
}

// unsigned short cksum(IcmpPacket *packet, int len){
//        long sum = 0;  /* assume 32 bit long, 16 bit short */

//        while(len > 1){
//          sum += *((unsigned short*) packet)++;
//          if(sum & 0x80000000)   /* if high order bit set, fold */
//            sum = (sum & 0xFFFF) + (sum >> 16);
//          len -= 2;
//        }

//        if(len)       /* take care of left over byte */
//          sum += (unsigned short) *(unsigned char *)packet;

//        while(sum>>16)
//          sum = (sum & 0xFFFF) + (sum >> 16);

//        return ~sum;
//      }

int socket = -1;


int socket_ping(char *host) {
	SceNetInAddr dst_addr;
	SceNetSockaddr from_addr;
	SceNetSockaddrIn serv_addr;
	int ret;
	IcmpUnion icmp; /* icmp union */
	TcpUnion tcp;

	sceNetInetPton(SCE_NET_AF_INET, host, (void*)&dst_addr);
	if (socket == -1) {
		socket = sceNetSocket("ping", SCE_NET_AF_INET, SCE_NET_SOCK_RAW, SCE_NET_IPPROTO_ICMP);
	}
	if (socket < 0) {
		printf("Error: Could not create ICMP socket.\n");
		return -1;
	}
	icmp.icmp_struct.hdr.type = SCE_NET_ICMP_TYPE_ECHO_REQUEST; /* set icmp type to echo request */
	icmp.icmp_struct.hdr.code = SCE_NET_ICMP_CODE_DEST_UNREACH_NET_UNREACH;
	icmp.icmp_struct.hdr.un.echo.id = 0x1; /* arbitrary id */
	icmp.icmp_struct.hdr.un.echo.sequence = 0x1234; /* arbitrary sequence */
	icmp.icmp_struct.hdr.checksum = 0;

	strncpy(icmp.icmp_struct.payload, "Random Payload in ping", (32));
	icmp.icmp_struct.hdr.checksum = in_cksum(icmp.icmp_u16buff, sizeof(IcmpPacket));

	serv_addr.sin_family = SCE_NET_AF_INET; /* set packet to IPv4 */
	serv_addr.sin_addr = dst_addr; /* set destination address */
	memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); /* fill sin_zero with zeroes */
	// Sending
	ret = sceNetSendto(socket, icmp.icmp_packet, sizeof(IcmpPacket), 0, (SceNetSockaddr*)&serv_addr, sizeof(SceNetSockaddr));
	// debug("Sending data to", host);
	if (ret < 1) {
		printf("Error: Could not send PING data.\n");
		return -2;
	}
	// debug("Successfully sent data to", host);
	uint32_t from_len = sizeof(from_addr);
	// Setting timeout
	int timeout = 2000;
	ret = sceNetSetsockopt(socket,
                     SCE_NET_SOCK_RAW, 
					 SCE_NET_SO_RCVTIMEO, &timeout, sizeof(timeout));
	printf("setTimeout ret is %i\n", ret);

	ret = sceNetSetsockopt(socket,
                     SCE_NET_SOCK_RAW, 
					 SCE_NET_SO_KEEPALIVE, &timeout, sizeof(timeout));
	printf("setTimeout ret is %i\n", ret);

	// debug("Waiting from an answer from", host);
	// Receiving

	ret = sceNetRecvfrom(socket, icmp.icmp_packet, sizeof(IcmpPacket), SCE_NET_MSG_DONTWAIT, &from_addr, (unsigned int*)&from_len);
	
	if (ret < 1) {
		printf("Error: Could not receive ICMP data\n");
		return -3;
	}
	// sceNetSocketClose(socket);
	return ret;
	// debug("Got an answer from", host);

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
    return sceNetConnect(socket, (SceNetSockaddr*)&addr, sizeof(addr));
}

/**
 * Set a timeout for connecting to a TCP server.
 *
 * @param socket The socket descriptor.
 * @param server The server address structure.
 * @param timeout The timeout value in milliseconds.
 * @return 0 on success, -5 on timeout, -6 on RST and other values on other errors.
 */
int socket_tcp_ping(int socket, char *ip, int port) {
	struct SceNetSockaddrIn addr = { 0 };

    addr.sin_len = sizeof(addr);
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_port = sceNetHtons(port);
    sceNetInetPton(SCE_NET_AF_INET, ip, &addr.sin_addr);

    int result;
    int epoll_fd;
    SceNetEpollEvent event = {0};
    int flags;

	// We force create a new socket for debug
    socket = sceNetSocket("debug", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
    if (socket < 0) {
		debug("Failed to create socket\n", NULL);
		return -7;
    }

    // Set the socket to non-blocking mode
    flags = 1; // Non-blocking flag
    result = sceNetSetsockopt(socket, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &flags, sizeof(flags));
    if (result < 0) {
		sceNetSocketClose(socket);
        return -1; // Failed to set non-blocking mode
    }

    // Attempt to connect
    result = sceNetConnect(socket, (SceNetSockaddr *)&addr, sizeof(addr));
    if (result < 0 && result != -2143223516) { // Ignore EINPROGRESS
		// debug("sceNetConnect failed?!", NULL);
		sceNetSocketClose(socket);
        return -2; // Failed to connect
    }

    // Create an epoll instance
    epoll_fd = sceNetEpollCreate("connect_epoll", 0);
    if (epoll_fd < 0) {
		// debug("Could not create epoll.", NULL);
		sceNetSocketClose(socket);
        return -3; // Failed to create epoll instance
    }

    // Add the socket to the epoll instance
    event.events |= SCE_NET_EPOLLOUT; // Monitor for writable state
    // event.data.u32 = 0;
	event.data.fd = socket;
    result = sceNetEpollControl(epoll_fd, SCE_NET_EPOLL_CTL_ADD, socket, &event);
    if (result < 0) {
        sceNetEpollDestroy(epoll_fd);
		sceNetSocketClose(socket);
		// debug("Could not add socket to epoll.", NULL);
        return -4; // Failed to add socket to epoll
    }

    // Wait for the connection to be established or timeout
	// 1 * 1000000 = 1 Second
	// 300000 = 1/3rd of a second
	// 100000 = 1/10th of a second
	
    result = sceNetEpollWait(epoll_fd, &event, 1, 100000); // epoll, events, max_events
    if (result <= 0) {
        sceNetEpollDestroy(epoll_fd);
		sceNetSocketClose(socket);
        return -5; // Connection timed out or error
    }

    // Check if the connection was successful
    int error = 0;
    unsigned int len = sizeof(error);
    result = sceNetGetsockopt(socket, SCE_NET_SOL_SOCKET, SCE_NET_SO_ERROR, &error, &len);
    if (result < 0 || error != 0) {
        sceNetEpollDestroy(epoll_fd);
		sceNetSocketClose(socket);
		// debug("Connection failed, all OK but timeout or RST.", NULL);
        return -6; // Connection failed
    }

    // Clean up epoll instance
    sceNetEpollDestroy(epoll_fd);
	// debug("Connection seems to have succeeded.", NULL);
	sceNetSocketClose(socket);
    return 0; // Success
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

