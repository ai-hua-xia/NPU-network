#include "Network_IPV4_send.h"
#include "Resource.h"
#include "Ethernet.h"
#include "ARP_Cache_Table.h"
#include "Network_ARP_send.h"
#include "Network_ARP_recv.h"

// 全局变量
u_int16_t ip_packet_id = 0; // IP数据包ID
u_int32_t ip_size_of_packet = 0; // IP数据包大小

// 外部变量声明
extern int ethernet_upper_len;
extern u_int8_t broadcast_mac[6];
extern u_int8_t local_ip[4];
extern u_int8_t target_ip[4];
extern u_int8_t netmask[4];
extern u_int8_t gateway_ip[4];
extern pcap_t *handle;
extern u_int8_t local_mac[6];

// 计算IP头部校验和
u_int16_t calculate_check_sum(ip_header *ip_hdr, int len)
{
    int sum = 0, tmp = len;
    u_int16_t *p = (u_int16_t*)ip_hdr;
    while (len > 1)
    {
        sum += *p;
        len -= 2;
        p++;
    }

    // 如果有一个字节剩余
    if (len)
    {
        sum += *((u_int8_t*)ip_hdr + tmp - 1);
    }

    // 将32位的和折叠成16位
    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}

// 加载IP头部
void load_ip_header(u_int8_t *ip_buffer)
{
    struct ip_header *ip_hdr = (struct ip_header*)ip_buffer;
    ip_size_of_packet = 0;
    // 初始化IP头部
    ip_hdr->version_hdrlen = 0x4f; // 0100 1111 表示IP版本4和头部长度60字节
    ip_hdr->type_of_service = 0xfe; /* 111 1 1110: 前3位: 优先级,
                                   然后1位: 延迟, 1位: 吞吐量, 1位: 可靠性
                                   1位: 路由成本, 1位: 未使用
                                   */
    ip_hdr->total_length = 0; // 等待数据长度，目前为0
    ip_hdr->id = ip_packet_id; // 标识
    ip_hdr->fragment_offset = 0x0000; /* 0 0 0 0 00...00: 前3位是标志: 1位: 0表示最后一个片段,
                                     1表示还有更多片段. 1位: 0允许分片, 1不允许分片. 1位: 未使用
                                     后12位是偏移量
                                     */
    ip_hdr->time_to_live = 64; // 默认1000ms
    ip_hdr->upper_protocol_type = IPPROTO_TCP; // 默认上层协议是TCP
    ip_hdr->check_sum = 0; // 初始为零

    int i;
    for (i = 0; i < 4; i++)
    {
        ip_hdr->source_ip[i] = local_ip[i];
        ip_hdr->destination_ip[i] = target_ip[i];
    }

    // 初始校验和与偏移量相关，因此在数据中需要计算校验和
    ip_size_of_packet += sizeof(ip_header);
}

// 加载IP数据
void load_ip_data(u_int8_t *ip_buffer, FILE *fp, int len)
{
    int i = 0;
    char ch;
    while (i < len && (ch = fgetc(fp)) != EOF)
    {
        *(ip_buffer + i) = ch;
        i++;
    }
    ip_size_of_packet += len;
}

// 检查是否在同一局域网
int is_same_lan(u_int8_t *local_ip, u_int8_t *destination_ip)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if ((local_ip[i] & netmask[i]) != (destination_ip[i] & netmask[i]))
            return 0;
    }
    return 1;
}

// 发送IP数据包
int network_ipv4_send(u_int8_t *ip_buffer, FILE *fp)
{
    // 获取文件大小
    int file_len;
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    rewind(fp);

    // 计算需要多少个片段
    int number_of_fragment = (int)ceil(file_len * 1.0 / MAX_IP_PACKET_SIZE);
    u_int16_t offset = 0;
    int ip_data_len;
    u_int16_t fragment_offset;
    while (number_of_fragment)
    {
        load_ip_header(ip_buffer);
        struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
        if (number_of_fragment == 1)
        {
            fragment_offset = 0x0000; // 最后一个片段
            ip_data_len = file_len - offset;
        }
        else
        {
            fragment_offset = 0x2000; // 允许下一个片段
            ip_data_len = MAX_IP_PACKET_SIZE;
        }

        fragment_offset |= ((offset / 8) & 0x0fff);
        ip_hdr->fragment_offset = htons(fragment_offset);

        ip_hdr->total_length = htons(ip_data_len + sizeof(ip_header));
        ip_hdr->check_sum = calculate_check_sum(ip_hdr, 60);
        load_ip_data(ip_buffer + sizeof(ip_header), fp, ip_data_len);

        // 检查目标PC的MAC地址是否在ARP表中
        u_int8_t *destination_mac = is_existed_ip(ip_hdr->destination_ip);
        if (destination_mac == NULL)
        {
            // 检查目标PC和本地主机是否在同一局域网
            if (is_same_lan(local_ip, ip_hdr->destination_ip))
            {
                // 发送ARP请求以获取目标IP的MAC地址
                network_arp_send(ip_hdr->destination_ip, broadcast_mac);
            }
            else
            {
                // 发送ARP请求以获取网关IP的MAC地址
                network_arp_send(gateway_ip, broadcast_mac);
            }

            // 等待回复，获取目标MAC地址
            struct pcap_pkthdr *pkt_hdr; // 数据包头部结构体指针
            u_int8_t *pkt_content; // 数据包内容指针

            // 循环等待捕获数据包
            while (pcap_next_ex(handle, &pkt_hdr, (const u_char **)&pkt_content) != 0)
            {
                destination_mac = NULL; // 初始化目标MAC地址为NULL
                struct ethernet_header *ethernet_hdr = (struct ethernet_header *)(pkt_content); // 将数据包内容转换为以太网头部结构体指针

                // 检查是否是可接受的数据包
                if (ntohs(ethernet_hdr->ethernet_type) != ETHERNET_ARP) continue; // 如果不是ARP数据包，继续等待下一个数据包

                int i;
                // 检查数据包的目标MAC地址是否是本地MAC地址
                for (i = 0; i < 6; i++)
                {
                    if (ethernet_hdr->destination_mac[i] != local_mac[i]) break; // 如果目标MAC地址不匹配，跳出循环
                }
                if (i < 6) continue; // 如果目标MAC地址不匹配，继续等待下一个数据包

                // 根据以太网类型处理数据包
                switch (ntohs(ethernet_hdr->ethernet_type))
                {
                case ETHERNET_ARP:
                    // 如果是ARP数据包，调用network_arp_recv函数处理，并获取目标MAC地址
                    destination_mac = network_arp_recv(pkt_content + sizeof(struct ethernet_header));
                    break;
                case ETHERNET_RARP:
                    // 如果是RARP数据包，不做处理
                    break;
                }

                // 如果成功获取到目标MAC地址，跳出循环
                if (destination_mac != NULL)
                    break;
            }
        }

        // 发送数据
        ethernet_upper_len = ip_size_of_packet; // IP数据包大小
        ethernet_send_packet(ip_buffer, destination_mac, ETHERNET_IP);

        offset += MAX_IP_PACKET_SIZE;
        number_of_fragment--;
    }

    fclose(fp);

    // 自动增加一个
    ip_packet_id++;

    return 1;
}