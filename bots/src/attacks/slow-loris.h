#ifndef SLOW_LR
#define SLOW_LR 


#include <pthread.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ctype.h>
#include "slow-loris.h"


void *flood_SL(void *par);
int fnAttackInformation(int attackID);
void flood_slow_loris(char *url_to_attack, int num_threads, char *file_pr, int f_read);

#endif