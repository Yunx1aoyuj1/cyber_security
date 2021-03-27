#include <stdio.h> 
#include <stdlib.h>    
#include <string.h>    
#include <unistd.h>
#include <errno.h>
#include <netinet/ip_icmp.h>   
#include <netinet/udp.h>   
#include <netinet/tcp.h>   
#include <netinet/ip.h>    
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

void ProcessPacket(unsigned char *, int);

void print_ether_header(unsigned char *buffer, int size);

void print_ip_header(unsigned char *, int);

void print_tcp_packet(unsigned char *, int);

void print_udp_packet(unsigned char *, int);

void print_icmp_packet(unsigned char *, int);

void PrintData(unsigned char *, int);

int sock_raw;
FILE *logfile;
int tcp = 0, udp = 0, icmp = 0, others = 0, igmp = 0, total = 0, i, j;
struct sockaddr_in source, dest;

int main() {
    int saddr_size, data_size;
    struct sockaddr saddr;
    struct in_addr in;

    unsigned char *buffer = (unsigned char *) malloc(2048); 

    logfile = fopen("log.txt", "w");
    if (logfile == NULL) 
        printf("Unable to create file.");
    printf("Starting...\n");
    
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        printf("Socket Error: %s\n", strerror(errno));
        return 1;
    }
    while (1) {
        saddr_size = sizeof saddr;
        
        data_size = recvfrom(sock_raw, buffer, 2048, 0, &saddr, (socklen_t *) &saddr_size);
        if (data_size < 0) {
            printf("Recvfrom error , failed to get packets\n");
            return 1;
        }
        
        ProcessPacket(buffer, data_size);
    }
    close(sock_raw);
    printf("Finished");
    return 0;
}

void ProcessPacket(unsigned char *buffer, int size) {
    print_ether_header(buffer, size);
    buffer = buffer + sizeof(struct ether_header);
    size = size - sizeof(struct ether_header);
    
    print_ip_header(buffer, size);
    struct iphdr *iph = (struct iphdr *) buffer;
    ++total;
    switch (iph->protocol) 
    {
        case 1:  
            ++icmp;
            print_icmp_packet(buffer, size);
            break;
        case 2:  
            ++igmp;
            break;
        case 6:  
            ++tcp;
            print_tcp_packet(buffer, size);
            break;
        case 17: 
            ++udp;
            print_udp_packet(buffer, size);
            break;
        default: 
            ++others;
            break;
    }
    printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp, udp, icmp, igmp, others,
           total);
    fprintf(logfile, "\n###########################################################");
    fflush(logfile);
    fflush(stdout);
}

void print_ether_header(unsigned char *buffer, int size) {
    struct ether_header *ethhdr = (struct ether_header *) buffer;

    fprintf(logfile, "\n");
    fprintf(logfile, "MAC Header\n");
    fprintf(logfile, "   |-Destination Mac   : %02x:%02x:%02x:%02x:%02x:%02x\n",
            ethhdr->ether_dhost[0], ethhdr->ether_dhost[1], ethhdr->ether_dhost[2], ethhdr->ether_dhost[3],
            ethhdr->ether_dhost[4], ethhdr->ether_dhost[5]);
    fprintf(logfile, "   |-Source Mac        : %02x:%02x:%02x:%02x:%02x:%02x\n",
            ethhdr->ether_shost[0], ethhdr->ether_shost[1], ethhdr->ether_shost[2], ethhdr->ether_shost[3],
            ethhdr->ether_shost[4], ethhdr->ether_shost[5]);
    fprintf(logfile, "   |-Protocol          : %d\n", ethhdr->ether_type);
}

void print_ip_header(unsigned char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *) Buffer;
    iphdrlen = iph->ihl * 4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(logfile, "\n");
    fprintf(logfile, "IP Header\n");
    fprintf(logfile, "   |-IP Version        : %d\n", (unsigned int) iph->version);
    fprintf(logfile, "   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int) iph->ihl,
            ((unsigned int) (iph->ihl)) * 4);
    fprintf(logfile, "   |-Type Of Service   : %d\n", (unsigned int) iph->tos);
    fprintf(logfile, "   |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    fprintf(logfile, "   |-Identification    : %d\n", ntohs(iph->id));



    fprintf(logfile, "   |-TTL      : %d\n", (unsigned int) iph->ttl);
    fprintf(logfile, "   |-Protocol : %d\n", (unsigned int) iph->protocol);
    fprintf(logfile, "   |-Checksum : %d\n", ntohs(iph->check));
    fprintf(logfile, "   |-Source IP        : %s\n", inet_ntoa(source.sin_addr));
    fprintf(logfile, "   |-Destination IP   : %s\n", inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *) Buffer;
    iphdrlen = iph->ihl * 4;
    struct tcphdr *tcph = (struct tcphdr *) (Buffer + iphdrlen);

    fprintf(logfile, "\n");
    fprintf(logfile, "TCP Header\n");
    fprintf(logfile, "   |-Source Port      : %u\n", ntohs(tcph->source));
    fprintf(logfile, "   |-Destination Port : %u\n", ntohs(tcph->dest));
    fprintf(logfile, "   |-Sequence Number    : %u\n", ntohl(tcph->seq));
    fprintf(logfile, "   |-Acknowledge Number : %u\n", ntohl(tcph->ack_seq));
    fprintf(logfile, "   |-Header Length      : %d DWORDS or %d BYTES\n", (unsigned int) tcph->doff,
            (unsigned int) tcph->doff * 4);
    fprintf(logfile, "   |-Urgent Flag          : %d\n", (unsigned int) tcph->urg);
    fprintf(logfile, "   |-Acknowledgement Flag : %d\n", (unsigned int) tcph->ack);
    fprintf(logfile, "   |-Push Flag            : %d\n", (unsigned int) tcph->psh);
    fprintf(logfile, "   |-Reset Flag           : %d\n", (unsigned int) tcph->rst);
    fprintf(logfile, "   |-Synchronise Flag     : %d\n", (unsigned int) tcph->syn);
    fprintf(logfile, "   |-Finish Flag          : %d\n", (unsigned int) tcph->fin);
    fprintf(logfile, "   |-Window         : %d\n", ntohs(tcph->window));
    fprintf(logfile, "   |-Checksum       : %d\n", ntohs(tcph->check));
    fprintf(logfile, "   |-Urgent Pointer : %d\n", tcph->urg_ptr);
    fprintf(logfile, "\n");
    fprintf(logfile, "                        DATA Dump                         ");
    fprintf(logfile, "\n");
    fprintf(logfile, "Data Payload\n");
    PrintData(Buffer + iphdrlen + tcph->doff * 4, (Size - tcph->doff * 4 - iph->ihl * 4));
}

void print_udp_packet(unsigned char *Buffer, int Size) {

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *) Buffer;
    iphdrlen = iph->ihl * 4;
    struct udphdr *udph = (struct udphdr *) (Buffer + iphdrlen);

    fprintf(logfile, "\nUDP Header\n");
    fprintf(logfile, "   |-Source Port      : %d\n", ntohs(udph->source));
    fprintf(logfile, "   |-Destination Port : %d\n", ntohs(udph->dest));
    fprintf(logfile, "   |-UDP Length       : %d\n", ntohs(udph->len));
    fprintf(logfile, "   |-UDP Checksum     : %d\n", ntohs(udph->check));
    fprintf(logfile, "Data Payload\n");
    PrintData(Buffer + iphdrlen + sizeof udph, (Size - sizeof udph - iph->ihl * 4));
}

void print_icmp_packet(unsigned char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *) Buffer;
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *) (Buffer + iphdrlen);

    fprintf(logfile, "\nICMP Header\n");
    fprintf(logfile, "   |-Type : %d", (unsigned int) (icmph->type));

    if ((unsigned int) (icmph->type) == 11)
        fprintf(logfile, "  (TTL Expired)\n");
    else if ((unsigned int) (icmph->type) == ICMP_ECHOREPLY)
        fprintf(logfile, "  (ICMP Echo Reply)\n");
    fprintf(logfile, "   |-Code : %d\n", (unsigned int) (icmph->code));
    fprintf(logfile, "   |-Checksum : %d\n", ntohs(icmph->checksum));
    fprintf(logfile, "\n");
    fprintf(logfile, "Data Payload\n");
    PrintData(Buffer + iphdrlen + sizeof icmph, (Size - sizeof icmph - iph->ihl * 4));
}

void PrintData(unsigned char *data, int Size) {

    for (i = 0; i < Size; i++) {
        if (i != 0 && i % 16 == 0) {
            fprintf(logfile, "         ");
            for (j = i - 16; j < i; j++) {
                if (data[j] >= 32 && data[j] <= 128)
                    fprintf(logfile, "%c", (unsigned char) data[j]); 

                else fprintf(logfile, "."); 
            }
            fprintf(logfile, "\n");
        }

        if (i % 16 == 0) fprintf(logfile, "   ");
        fprintf(logfile, " %02X", (unsigned int) data[i]);

        if (i == Size - 1) {
            for (j = 0; j < 15 - i % 16; j++) fprintf(logfile, "   "); 

            fprintf(logfile, "         ");

            for (j = i - i % 16; j <= i; j++) {
                if (data[j] > 31 && data[j] < 128) fprintf(logfile, "%c", (unsigned char) data[j]);
                else fprintf(logfile, ".");
            }
            fprintf(logfile, "\n");
        }
    }
}