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
    if((s = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
        perror("Failed to create socket");
        exit(1);
    }
    printf("Socket = %d\n", s);
    return s;
}

int connect_to(int s, char *ip_str, short port, struct sockaddr_in *server)
{
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = inet_addr(ip_str);
    memset(server->sin_zero, '\0', sizeof(server->sin_zero));

    if (connect(s, (struct sockaddr *)server ,sizeof(struct sockaddr)) == ERROR){
        perror("Unable to connect ");
        exit(1);
    }
    printf("somr=thing %d \n", server->sin_addr.s_addr);
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
