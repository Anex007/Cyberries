#pragma once

void init_rand(uint32_t x);
uint32_t rand_cmwc(void);
unsigned short csum (unsigned short *buf, int nwords);
void setup_ip_header(struct iphdr *iph);
void setup_tcp_header(struct tcphdr *tcph);
void *flood(void *par1);
void flood_with_syn(char *ip, short port, int thrds, int f_read);
