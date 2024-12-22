#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/if.h>
#include <linux/route.h>
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
	// Not implemented, probably not absolutely necessary at the moment
	return -1;
}

/**
 * Perform a TCP ping (connect with timeout) to a specified IP and port.
 *
 * @param sock The socket descriptor (will be created if -1 is passed).
 * @param ip The target IP address as a string.
 * @param port The target port.
 * @return 0 on success, negative values on failure.
 */
int socket_tcp_ping(int sock, char *ip, int port) {
    struct sockaddr_in addr = { 0 };
    int timeout = 100; // Timeout in milliseconds (converted to microseconds later)

    // Set up the address structure
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        debug("Invalid IP address\n", NULL);
        return -7; // Invalid IP address
    }

    int result;
    int epoll_fd;
    struct epoll_event event = {0};
    int flags;

    // If the socket is not provided, create a new one
    if (sock < 0) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            debug("Failed to create socket\n", NULL);
            return -7; // Failed to create socket
        }
    }

    // Set the socket to non-blocking mode
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        close(sock);
        return -1; // Failed to get socket flags
    }
    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) < 0) {
        close(sock);
        return -1; // Failed to set non-blocking mode
    }

    // Attempt to connect
    result = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (result < 0 && errno != EINPROGRESS) {
        close(sock);
        return -2; // Failed to connect
    }

    // Create an epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        close(sock);
        debug("Could not create epoll.\n", NULL);
        return -3; // Failed to create epoll instance
    }

    // Add the socket to the epoll instance
    event.events = EPOLLOUT; // Monitor for writable state
    event.data.fd = sock;
    result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &event);
    if (result < 0) {
        close(epoll_fd);
        close(sock);
        debug("Could not add socket to epoll.\n", NULL);
        return -4; // Failed to add socket to epoll
    }

    // Wait for the connection to be established or timeout
    struct epoll_event events[1];
    result = epoll_wait(epoll_fd, events, 1, timeout);
    if (result <= 0) {
        close(epoll_fd);
        close(sock);
        debug("Connection timed out or error.\n", NULL);
        return -5; // Connection timed out or error
    }

    // Check if the connection was successful
    int error = 0;
    socklen_t len = sizeof(error);
    result = getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
    if (result < 0 || error != 0) {
        close(epoll_fd);
        close(sock);
        debug("Connection failed, all OK but timeout or RST.\n", NULL);
        return -6; // Connection failed
    }

    // Clean up epoll instance
    close(epoll_fd);
    debug("Connection seems to have succeeded.\n", NULL);
    close(sock);
    return 0; // Success
}

/**
 * Get network configuration information (IP, netmask, MAC, gateway, state).
 *
 * @return Pointer to a NetConfig structure on success, NULL on failure.
 */
NetConfig *get_device_info() {
    NetConfig *device_config = malloc(sizeof(NetConfig));

    if (!device_config) {
        debug("Failed to allocate memory for device config.\n", NULL);
        return NULL;
    }

    memset(device_config, 0, sizeof(NetConfig));

    // Get network interface information using getifaddrs
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        debug("Failed to get network interface information.\n", NULL);
        free(device_config);
        return NULL;
    }

    // Iterate through the interfaces to find the first active one

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET) {
            continue; // Skip non-IPv4 interfaces
        }
        // Get IP address
        struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
        inet_ntop(AF_INET, &addr->sin_addr, device_config->ip_addr, sizeof(device_config->ip_addr));

        // Get netmask
        struct sockaddr_in *netmask = (struct sockaddr_in *)ifa->ifa_netmask;
        inet_ntop(AF_INET, &netmask->sin_addr, device_config->netmask, sizeof(device_config->netmask));

        // Get MAC address
        struct ifreq ifr;
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            debug("Failed to create socket for MAC address retrieval.\n", NULL);
            freeifaddrs(ifaddr);
            free(device_config);
            return NULL;
        }
        strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ);
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
            memcpy(device_config->ether_addr, ifr.ifr_hwaddr.sa_data, 6);
        }
        close(sock);

        // Assume the interface is connected (state = 1)
        device_config->state = 1;

    }

    // Get default gateway (requires parsing /proc/net/route)
    FILE *fp = fopen("/proc/net/route", "r");
    if (fp) {
        char iface[16], dest[16], gw[16];
        uint32_t flags;
		int ret;
        while ((ret = fscanf(fp, "%15s %15s %15s %X", iface, dest, gw, &flags) >= 0)) {
            if (strcmp(dest, "00000000") == 0) {
                inet_ntop(AF_INET, &gw, device_config->gateway, sizeof(device_config->gateway));
				device_config->state = 3;
            }
        }
        fclose(fp);
    } else {
        debug("Failed to open /proc/net/route for default gateway.\n", NULL);
    }

    freeifaddrs(ifaddr);
    return device_config;
}