#include "Network_ARP_send.h" // 包含ARP发送相关的头文件
#include "Ethernet.h"         // 包含以太网相关的头文件
#include "Resource.h"         // 包含资源相关的头文件

// 外部变量声明
// 这些变量通常会在其他地方定义，这里只是声明它们是外部的，可以在这个文件中使用。
extern u_int8_t local_mac[6]; // 本地MAC地址
extern u_int8_t local_ip[4];  // 本地IP地址
extern int ethernet_upper_len; // 以太网上层协议的数据长度

// ARP缓冲区
u_int8_t arp_buffer[MAX_SIZE]; // 用于存储ARP数据包的缓冲区

// 加载ARP数据包到缓冲区准备发送
void load_arp_packet(u_int8_t *destination_ip)
{
    // 将arp_buffer转换为ARP数据包结构体指针
    struct arp_pkt *arp_packet = (struct arp_pkt *)(arp_buffer);

    // 设置硬件类型（以太网）
    arp_packet->hardware_type = htons(ARP_HARDWARE); // 使用htons确保字节顺序正确

    // 设置协议类型（IPv4）
    arp_packet->protocol_type = htons(ETHERNET_IP); // 使用htons确保字节顺序正确

    // 设置硬件地址长度（以太网MAC地址长度为6字节）
    arp_packet->hardware_addr_length = 6;

    // 设置协议地址长度（IPv4地址长度为4字节）
    arp_packet->protocol_addr_length = 4;

    // 设置操作码为ARP请求
    arp_packet->op_code = htons(ARP_REQUEST); // 使用htons确保字节顺序正确

    // 复制本地MAC地址到源MAC地址字段
    int i;
    for (i = 0; i < 6; i++)
    {
        arp_packet->source_mac[i] = local_mac[i];
    }

    // 复制本地IP地址到源IP地址字段
    for (i = 0; i < 4; i++)
    {
        arp_packet->source_ip[i] = local_ip[i];
    }

    // 目标MAC地址暂时未知，填充为全0
    for (i = 0; i < 6; i++)
    {
        arp_packet->destination_mac[i] = 0x00;
    }

    // 复制目标IP地址到目标IP地址字段
    for (i = 0; i < 4; i++)
    {
        arp_packet->destination_ip[i] = destination_ip[i];
    }
}

// 发送ARP数据包
void network_arp_send(u_int8_t *destination_ip, u_int8_t *ethernet_dest_mac)
{
    // 加载ARP数据包到缓冲区
    load_arp_packet(destination_ip);

    // 设置上层协议的数据长度
    ethernet_upper_len = sizeof(struct arp_pkt); // ARP数据包的大小

    // 通过以太网发送ARP数据包
    ethernet_send_packet(arp_buffer, ethernet_dest_mac, ETHERNET_ARP);
}