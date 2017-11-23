#ifndef HANDLE_ME
	#define HANDLE_ME


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>    //for socket ofcourse
#include <errno.h> //For errno - the error number
#include <arpa/inet.h>
#include <netinet/in.h>

#define ERROR -1

int make_socket(void);
void *accept_handler(void *arg);
void send_to_bots(char *data);
void bind_socket(int sock, short port, char *ip);
void close_socket(int sock_);

typedef struct connections
{
	int sock;
	struct sockaddr_in client_conn;
	struct connections *next;
} CONNS;

#endif