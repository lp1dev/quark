#include <stdio.h>
#include "net.h"

static int socket = -1;

void debug_init() {
    int ret;

    if (socket == -1) {
        socket = socket_create_tcp("debugger");
        
        ret = socket_connect(socket, "192.168.129.12", 4444);
    }
    if (ret < 0) {
        socket = -1;
        printf("Warning: Could not connect to 192.168.129.12 4444\n");
    } else {
        socket_send_str(socket, "[Quark :: Debug Socket Initialized]\n");
    }
}

int debug(char *msg, char *item) {
    printf("DEBUG: %s [%s]\n", msg, item);
    if (socket > 0) {
        socket_send_str(socket, msg);
        if (item != NULL) {
            socket_send_str(socket, item);
        }
        socket_send_str(socket, "\n");
    } else {
        printf("Warning: Debug socket not initialized\n");
    }
}