#include "packet.h"

extern int main_sock;

int make_socket(void)
{
    int s;
    if((s = socket(AF_INET , SOCK_STREAM , 0)) == ERROR) {
        perror("Failed to create socket");
        _exit(1);
    }
    return s;
}