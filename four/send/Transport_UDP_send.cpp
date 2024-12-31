#include "Transport_UDP_send.h"
#include "Network_IPV4_send.h"
#include "Network_ARP_send.h"
#include "Network_ARP_recv.h"
#include "Ethernet.h"
#include "ARP_Cache_Table.h"
#include "Resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

extern u_int8_t target_ip[4];
extern u_int8_t local_ip[4];
extern u_int8_t broadcast_mac[6];
extern u_int8_t gateway_ip[4];
extern pcap_t *handle;
extern int ip_data_len;

int create_socket() {
    int sockid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockid == INVALID_SOCKET) {
        printf("Failed to create socket. Error: %d\n", WSAGetLastError());
        return -1;
    }
    return sockid;
}

int bind_socket(int sockid, const char* ip, int port) {
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    int nResult = bind(sockid, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (nResult == SOCKET_ERROR) {
        printf("Failed to bind socket. Error: %d\n", WSAGetLastError());
        return -1;
    }
    return 0;
}

int sendto_socket(int sockid, const char* buf, int buflen, const struct sockaddr* destaddr, int addrlen) {
    printf("sendto_socket\n");
    printf("sendto_socket: sockid = %d, buflen = %d\n", sockid, buflen);
    const struct sockaddr_in* sin = reinterpret_cast<const struct sockaddr_in*>(destaddr);
    printf("Destination address: %s:%d\n", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
    int sent = sendto(sockid, buf, buflen, 0, destaddr, addrlen);
    if (sent == SOCKET_ERROR) {
        printf("Failed to send data. Error: %d\n", WSAGetLastError());
        return -1;
    }
    return sent;
}

int recvfrom_socket(int sockid, char* buf, int buflen, struct sockaddr* srcaddr, int* addrlen) {
    int received = recvfrom(sockid, buf, buflen, 0, srcaddr, addrlen);
    if (received == SOCKET_ERROR) {
        printf("Failed to receive data. Error: %d\n", WSAGetLastError());
        return -1;
    }
    return received;
}

int close_socket(int sockid) {
    return closesocket(sockid);
}

void transport_udp_send(u_int8_t *ip_buffer, u_int8_t protocol_type) {
    // ���ļ���ȡ����
    FILE *file = fopen("data_send.txt", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    // ��ȡ�ļ���С
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // ���仺��������ȡ�ļ�����
    char *data = (char *)malloc(file_size + 1);
    if (data == NULL) {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return;
    }
    fread(data, 1, file_size, file);
    data[file_size] = '\0'; // ȷ���Կ��ַ���β
    fclose(file);

    // �����׽���
    int sockid = create_socket();
    if (sockid == -1) {
        printf("Failed to create socket.\n");
        free(data);
        return;
    }

    // ��װĿ�ĵص�ַ
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(12345); // Ŀ�Ķ˿�
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Ŀ��IP��ַ������ʹ�ñ�����ַ��Ϊʾ��

    // ����sendto_socket������������
    int sent = sendto_socket(sockid, data, file_size, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent == -1) {
        printf("Failed to send data.\n");
    } else {
        printf("Data sent successfully. Sent %d bytes.\n", sent);
    }

    // ���ջ�Ӧ
    char recv_buf[1024];
    struct sockaddr srcaddr;
    int addrlen = sizeof(srcaddr);

    // ����UDP����
    int received = recvfrom_socket(sockid, recv_buf, sizeof(recv_buf), (struct sockaddr*)&srcaddr, &addrlen);
    if (received == -1) {
        printf("Failed to receive response.\n");
    } else {
        // ��ӡ���յ�������
        printf("Received %d bytes: %s\n", received, recv_buf);

        // ��ӡUDPͷ����Ϣ
        struct udphdr *udp_hdr = (struct udphdr *)(recv_buf);
        printf("--------------UDP Protocol-------------------\n");
        printf("UDP Source Port: %d\n", ntohs(udp_hdr->uh_sport));
        // printf("UDP Source Port_initial: %d\n", udp_hdr->uh_sport);
        printf("UDP Destination Port: %d\n", ntohs(udp_hdr->uh_dport));
        // printf("UDP Destination Port_initial: %d\n", udp_hdr->uh_dport);
        printf("UDP Length: %d\n", ntohs(udp_hdr->uh_ulen));
        printf("UDP Checksum: %04x\n", ntohs(udp_hdr->uh_sum));
        printf("-----------------End of UDP Protocol---------------\n");
    }

    // ����
    free(data);
    closesocket(sockid); // ʹ��closesocket�ر��׽��֣���ΪWinsock
}
