#include "Network_ARP_recv.h"
#include "Network_ARP_send.h"
#include "Resource.h"
#include "ARP_Cache_Table.h"

// 外部变量声明
extern u_int8_t local_mac[6]; // 本地MAC地址
extern u_int8_t local_ip[4];  // 本地IP地址
extern u_int8_t target_ip[4]; // 目标IP地址

// 检查是否接受ARP数据包
int is_accept_arp_packet(struct arp_pkt *arp_packet)
{
    // 检查硬件类型是否为以太网
    if (ntohs(arp_packet->hardware_type) != ARP_HARDWARE) return 0;
    // 检查协议类型是否为IPv4
    if (ntohs(arp_packet->protocol_type) != ETHERNET_IP) return 0;
    // 检查操作码是否为ARP回复
    if (ntohs(arp_packet->op_code) != ARP_REPLY) return 0;

    int i;
    // 检查目标MAC地址是否为本地MAC地址
    for (i = 0; i < 6; i++)
    {
        if (arp_packet->destination_mac[i] != local_mac[i]) return 0;
    }
    // 检查目标IP地址是否为本地IP地址
    for (i = 0; i < 4; i++)
    {
        if (arp_packet->destination_ip[i] != local_ip[i]) return 0;
    }
    // 检查源IP地址是否为目标IP地址
    for (i = 0; i < 4; i++)
    {
        if (arp_packet->source_ip[i] != target_ip[i]) return 0;
    }

    struct arp_node *element;
    // 如果源IP地址不存在于ARP表中，则创建并插入新的ARP节点
    if (!is_existed_ip(arp_packet->source_ip))
    {
        element = make_arp_node(arp_packet->source_ip, arp_packet->source_mac, STATIC_STATE);
        insert_arp_node(element);
    }

    return 1; // 接受ARP数据包
}

// 输出ARP数据包信息
void output(struct arp_pkt *arp_packet)
{
    printf("--------------ARP Protocol---------------\n");
    printf("Hardware Type: %04x\n", arp_packet->hardware_type);
    printf("Protocol Type: %04x\n", arp_packet->protocol_type);
    printf("Operation Code: %04x\n", arp_packet->op_code);
    printf("Source MAC: ");
    int i;
    for (i = 0; i < 6; i++)
    {
        if (i) printf("-");
        printf("%02x", arp_packet->source_mac[i]);
    }
    printf("\n");
    printf("Source IP: ");
    for (i = 0; i < 4; i++)
    {
        if (i) printf(".");
        printf("%d", arp_packet->source_ip[i]);
    }
    printf("\n");
}

// 接收并处理ARP数据包
u_int8_t* network_arp_recv(u_int8_t *arp_buffer)
{
    struct arp_pkt *arp_packet = (struct arp_pkt *)(arp_buffer);

    // 检查是否接受ARP数据包
    if (is_accept_arp_packet(arp_packet))
    {
        output(arp_packet); // 输出ARP数据包信息
        output_arp_table(); // 输出ARP表
        return arp_packet->source_mac; // 返回源MAC地址
    }
    return NULL; // 如果不接受ARP数据包，返回NULL
}