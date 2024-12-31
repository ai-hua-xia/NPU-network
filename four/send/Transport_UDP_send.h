#ifndef TRANSPORT_UDP_H
#define TRANSPORT_UDP_H

#include "Header_Include.h"
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

// UDP ͷ���ṹ����
struct udphdr {
    u_short uh_sport;        // Դ�˿�
    u_short uh_dport;        // Ŀ�Ķ˿�
    u_short uh_ulen;         // UDP ���ݱ�����
    u_short uh_sum;          // UDP ���ݱ�У���
};

int create_socket();
int bind_socket(int sockid, const char* ip, int port);
int sendto_socket(int sockid, const char* buf, int buflen, const struct sockaddr* destaddr, int addrlen);
int recvfrom_socket(int sockid, char* buf, int buflen, struct sockaddr* srcaddr, int* addrlen);
int close_socket(int sockid);
void transport_udp_send(u_int8_t *ip_buffer, u_int8_t protocol_type); // ȷ����������Ϊ u_int8_t*
u_int16_t calculate_udp_checksum(struct ip_header *ip_hdr, struct udphdr *udp_hdr); 

#endif // TRANSPORT_UDP_H
