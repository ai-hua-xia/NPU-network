#include "Transport_UDP.h"
#include "Network_IPV4_recv.h"
#include <stdio.h>
#include <winsock2.h>

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

void transport_udp_recv() {
    // 创建套接字
    int sockid = create_socket();
    if (sockid == -1) {
        printf("Failed to create socket.\n");
        return;
    }

    // 绑定套接字
    if (bind_socket(sockid, "0.0.0.0", 12345) != 0) { // 绑定到本地端口12345
        printf("Failed to bind socket.\n");
        close_socket(sockid);
        return;
    }

    // 准备接收缓冲区
    char recv_buf[1024];
    struct sockaddr srcaddr;
    int addrlen = sizeof(srcaddr);

    // 接收UDP数据
    printf("begin to recv\n");
    int received = recvfrom_socket(sockid, recv_buf, sizeof(recv_buf), &srcaddr, &addrlen);
    if (received == -1) {
        printf("Failed to receive data.\n");
        close_socket(sockid);
        return;
    }

    // 打印接收到的数据
    recv_buf[received] = '\0';
    printf("Received %d bytes: %s\n", received, recv_buf);

    // 将接收到的数据写入文件
    FILE *file = fopen("data_recv.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        close_socket(sockid);
        return;
    }
    fwrite(recv_buf, 1, received, file);
    fclose(file);
    printf("Data written to file successfully.\n");

    // 打印UDP头部信息
    struct udphdr *udp_hdr = (struct udphdr *)recv_buf;
    printf("--------------UDP Protocol-------------------\n");
    printf("UDP Source Port: %d\n", ntohs(udp_hdr->uh_sport));
    printf("UDP Destination Port: %d\n", ntohs(udp_hdr->uh_dport));
    printf("UDP Length: %d\n", ntohs(udp_hdr->uh_ulen));
    printf("UDP Checksum: %04x\n", ntohs(udp_hdr->uh_sum));
    printf("-----------------End of UDP Protocol---------------\n");

    // 发送响应
    const char* response = "Response from server";
    if (sendto_socket(sockid, response, strlen(response) + 1, &srcaddr, addrlen) == -1) {
        printf("Failed to send response.\n");
    }
    printf("Success to send udp response.\n");

    // 关闭套接字
    close_socket(sockid);
}
