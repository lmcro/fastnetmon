#include <stdio.h>
#include <iostream>
#include <pcap.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


/* 
   pcap dump format:
    global header: struct pcap_file_header
    packet header: struct pcap_pkthdr



*/
int pcap_reader(const char* pcap_file_path) {
    int filedesc = open(pcap_file_path, O_RDONLY);

    if (filedesc <= 0) {
        printf("Can't open dump file");
        return -1;
    } 

    struct pcap_file_header pcap_header;
    ssize_t file_header_readed_bytes = read(filedesc, &pcap_header, sizeof(struct pcap_file_header));

    if (file_header_readed_bytes != sizeof(struct pcap_file_header)) {
        printf("Can't read pcap file header");
    }

    // http://www.tcpdump.org/manpages/pcap-savefile.5.html
    if (pcap_header.magic == 0xa1b2c3d4 or pcap_header.magic == 0xd4c3b2a1) {
        // printf("Magic readed correctly\n");
    } else {
        printf("Magic in file header broken\n");
        return -2;
    }

    // Buffer for packets
    int buffer_size = 1024*1024;
    char packet_bufer[buffer_size];

    unsigned int read_packets = 0; 
    while (1) {
        struct pcap_pkthdr pcap_packet_header;
        ssize_t packet_header_readed_bytes = read(filedesc, &pcap_packet_header, sizeof(struct pcap_pkthdr));
      
        if (packet_header_readed_bytes != sizeof(struct pcap_pkthdr)) {
            // We haven't any packets 
            break;
        }

        printf("packet header read. snap len: %d bytes\n", pcap_packet_header.caplen);

        if (pcap_packet_header.caplen > buffer_size) {
            printf("Please enlarge packet buffer! We got packet with size: %d but our buffer is %d bytes\n",
                pcap_packet_header.caplen, buffer_size);
            return -4;
        }

        ssize_t packet_payload_readed_bytes = read(filedesc, packet_bufer, pcap_packet_header.caplen);
 
        if (pcap_packet_header.caplen != packet_payload_readed_bytes) {
            printf("I read packet header but can't read packet payload\n");
            return -3;
        }

        printf("packet payload read\n");
        
        read_packets++;
    }

    printf("I correctly read %d packets from this dump\n", read_packets);

    return 0;
}

int main() {
    pcap_reader("/Users/pavel-odintsov/Dropbox/ipfix_example_ipt_netflow_syn_flood.pcap");
}