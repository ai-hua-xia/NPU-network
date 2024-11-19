#include "Network_IPV4_recv.h"

#define MAX_DATA_SIZE 1000000
u_int16_t ip_id = 0; // IP数据包ID
u_int16_t i = 0; // 数据缓冲区索引

u_int8_t data_buffer[MAX_DATA_SIZE]; // 数据缓冲区

int previous = 0, current = 0; // 时间戳变量

extern u_int8_t local_ip[4]; // 本地IP地址

/*
如果允许分片，则存储到缓冲区直到不允许分片，然后存储到文件。
*/

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

// 检查是否接受IP数据包
int is_accept_ip_packet(struct ip_header *ip_hdr)
{
    int i;
    int flag = 0;
    for (i = 0; i < 4; i++)
    {
        if (ip_hdr->destination_ip[i] != local_ip[i]) break;
    }

    if (i == 4)
    {
        flag = 1;
        printf("It's sent to my IP.\n");
    }

    for (i = 0; i < 4; i++)
    {
        if (ip_hdr->destination_ip[i] != 0xff) break;
    }
    if (i == 4)
    {
        flag = 1;
        printf("It's broadcast IP.\n");
    }

    if (!flag)
        return 0;

    u_int16_t check_sum = calculate_check_sum(ip_hdr, 60);
    if (check_sum == 0xffff || check_sum == 0x0000)
    {
        printf("No error in ip_header.\n");
        return 1;
    }
    else
    {
        printf("Error in ip_header\n");
        return 0;
    }
}

// 将数据加载到缓冲区
void load_data_to_buffer(u_int8_t *buffer, u_int8_t *ip_data, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        *(buffer + i) = *(ip_data + i);
    }
}

// 将数据加载到文件
int load_data_to_file(u_int8_t *buffer, int len, FILE *fp)
{
    int res = fwrite(buffer, sizeof(u_int8_t), len, fp);
    if (res != len)
    {
        printf("Write file error!\n");
        return 0;
    }
    fflush(fp);
    return 1;
}

// 接收并处理IP数据包
int network_ipv4_recv(u_int8_t *ip_buffer)
{
    struct ip_header *ip_hdr = (struct ip_header *)ip_buffer; // 将缓冲区转换为IP头部结构体指针
    int len = ntohs(ip_hdr->total_length) - sizeof(ip_header); // 计算IP数据部分的长度

    // 检查数据包是否有效
    if (!is_accept_ip_packet(ip_hdr))
    {
        return 0; // 如果数据包无效，返回0
    }

    u_int16_t fragment;
    fragment = ntohs(ip_hdr->fragment_offset); // 获取分片偏移量

    int dural = 0;
    if (previous == 0)
    {
        previous = time(NULL); // 获取当前时间作为初始时间
    }
    else
    {
        // 获取当前时间
        current = time(NULL);
        dural = current - previous; // 计算时间间隔
        printf("%d %d\n", current, previous);
        // 将当前时间更新为上一次时间
        previous = current;
    }

    // 时间间隔不能超过30秒
    if (dural >= 30)
    {
        printf("Time Elapsed.\n");
        return 0; // 如果时间间隔超过30秒，返回0
    }

    // 如果有更多分片且分片ID相同
    if ((fragment & 0x2000) && (ip_id == ip_hdr->id))
    {
        // 将数据加载到缓冲区
        load_data_to_buffer(data_buffer + i, ip_buffer + sizeof(ip_header), len);
        i += len; // 更新缓冲区索引
        return 1; // 返回1表示成功接收分片
    }
    // 如果是最后一个分片且分片ID相同
    else if (ip_id == ip_hdr->id)
    {
        // 将数据加载到缓冲区
        load_data_to_buffer(data_buffer + i, ip_buffer + sizeof(ip_header), len);
        i += len; // 更新缓冲区索引
        FILE *fp = fopen("data_receive.txt", "w"); // 打开文件以写入数据
        if (load_data_to_file(data_buffer, i, fp))
        {
            printf("Load to file Succeed.\n"); // 如果成功写入文件，打印成功消息
        }
        fclose(fp); // 关闭文件
        // 恢复初始值
        i = 0;
        ip_id++;
    }
    else
    {
        printf("Lost packets.\n");
        // 跳过最后一个分片
        i = 0;
        ip_id++;
        return 0; // 返回0表示丢失分片
    }

    // 输出IP数据包信息
    printf("--------------IP Protocol-------------------\n");
    printf("IP version: %d\n", (ip_hdr->version_hdrlen & 0xf0) >> 4);
    printf("Type of service: %02x\n", ip_hdr->type_of_service);
    printf("IP packet length: %d\n", len + sizeof(ip_header));
    printf("IP identification: %d\n", ip_hdr->id);
    printf("IP fragment & offset: %04x\n", ntohs(ip_hdr->fragment_offset));
    printf("IP time to live: %d\n", ip_hdr->time_to_live);
    printf("Upper protocol type: %02x\n", ip_hdr->upper_protocol_type);
    printf("Check sum: %04x\n", ip_hdr->check_sum);
    printf("Source IP: ");
    for (i = 0; i < 4; i++)
    {
        if (i) printf(".");
        printf("%d", ip_hdr->source_ip[i]);
    }
    printf("\nDestination IP: ");
    for (i = 0; i < 4; i++)
    {
        if (i) printf(".");
        printf("%d", ip_hdr->destination_ip[i]);
    }
    printf("\n");

    u_int8_t upper_protocol_type = ip_hdr->upper_protocol_type;
    switch (upper_protocol_type)
    {
    case IPPROTO_TCP:
        // transport_tcp_recv(buffer);
        break;
    case IPPROTO_UDP:
        // transport_udp_recv(buffer);
        break;
    }

    printf("-----------------End of IP Protocol---------------\n");
    return 1; // 返回1表示成功处理数据包
}