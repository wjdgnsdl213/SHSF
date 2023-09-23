/*
- Ethernet Header: src mac / dst mac
- IP Header: src ip / dst ip
- TCP Header: src port / dst port
- Message
*/

#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include "myheader.h"


void got_packet(u_char *args, const struct pcap_pkthdr *header,
                              const u_char *packet)
{
  struct ethheader *eth = (struct ethheader *)packet;
  printf("-------------Ethernet header-------------\n");
  printf("    Src Mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],
           eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);

  printf("    Dst Mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],
           eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);

  if (ntohs(eth->ether_type) == 0x0800) { // 0x0800 is IP type
    struct ipheader *ip = (struct ipheader *)
                           (packet + sizeof(struct ethheader));
    printf("-------------IP header-------------\n");
    printf("       From: %s\n", inet_ntoa(ip->iph_sourceip));
    printf("         To: %s\n", inet_ntoa(ip->iph_destip));


    if(ip->iph_protocol == IPPROTO_TCP) {
        struct tcpheader *tcp = (struct tcpheader *)((unsigned char *)ip + (ip->iph_ihl & 0x0F) * 4);
        printf("-------------TCP header-------------\n");
        printf("   Src Port: %u\n", ntohs(tcp->tcp_sport));
        printf("   Dst Port: %u\n", ntohs(tcp->tcp_dport));

        int ip_header_len = (ip->iph_ihl & 0x0F) * 4;
        int tcp_header_len = TH_OFF(tcp) * 4;
        int data_len = ntohs(ip->iph_len) - ip_header_len - tcp_header_len;
        if (data_len > 0) {
                const unsigned char *message = packet + sizeof(struct ethheader) + ip_header_len + tcp_header_len;
                printf("-------------Message-------------\n");
                for (int i = 0; i<data_len; i++) {
                        printf("%c", message[i]);
                }

        }
        printf("\n");
    }
  }
}


int main()
{
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "tcp";
  bpf_u_int32 net;

  // Step 1: Open live pcap session on NIC with name enp0s3
  handle = pcap_open_live("ens33", BUFSIZ, 1, 1000, errbuf);

  // Step 2: Compile filter_exp into BPF psuedo-code
  pcap_compile(handle, &fp, filter_exp, 0, net);
  if (pcap_setfilter(handle, &fp) !=0) {
      pcap_perror(handle, "Error:");
      exit(EXIT_FAILURE);
  }

  // Step 3: Capture packets
  pcap_loop(handle, -1, got_packet, NULL);

  pcap_close(handle);   //Close the handle
  return 0;
}