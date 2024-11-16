#include<stdio.h>
#include<stdlib.h>

#define HAVE_REMOTE
#include<pcap.h>          // 引入pcap库，用于捕获和分析网络数据包
#include<WinSock2.h>      // 引入WinSock2库，用于Windows平台的网络编程

#pragma warning(disable:4996)  // 关闭编译器对某些不安全操作的警告，如fscanf等

// 捕获到数据包时调用的回调函数
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content);

// Ethernet协议头部结构
struct ethernet_header {
    u_int8_t ether_dhost[6]; // 目标MAC地址
    u_int8_t ether_shost[6]; // 源MAC地址
    u_int16_t ether_type;    // 以太网类型（用于识别上层协议）
};

// 定义两个待匹配的MAC地址，用于筛选目标数据包
u_int8_t accept_dest_mac[2][6] = { 
    { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 }, 
    { 0x00, 0x09, 0x73, 0x07, 0x74, 0x73 } 
};

// CRC32查找表
u_int32_t crc32_table[256];

// 生成CRC32查找表
void generate_crc32_table() {
    int i, j;
    u_int32_t crc;
    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;  // 使用多项式0xEDB88320生成CRC
            else
                crc >>= 1;  // 如果最低位是0，右移1位
        }
        crc32_table[i] = crc;  // 将计算出的CRC值存储到查找表中
    }
}

// 计算CRC32校验值
u_int32_t calculate_crc(u_int8_t *buffer, int len) {
    int i;
    u_int32_t crc = 0xffffffff;  // 初始化CRC值为0xFFFFFFFF
    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ buffer[i]];  // 按字节计算CRC值
    }
    crc ^= 0xffffffff;  // 对最终的CRC值进行取反
    return crc;
}

// 以太网协议解析回调函数
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content) {
    u_short ethernet_type;  //以太网类型字段
    struct ethernet_header *ethernet_protocol;  //指向以太网协议头部的指针
    u_char *mac_string;  //用于打印 MAC 地址的指针
    static int packet_number = 1;  // 用于计数捕获的包数

    ethernet_protocol = (struct ethernet_header*)packet_content;  // 将数据包内容转换为以太网协议头部结构体
    int len = packet_header->len;  // 获取数据包的总长度,包含头部和数据部分
    int i, j;

    // 检查是否是目标MAC地址或者广播包
    int flag = 2;
    for (i = 0; i < 2; i++) {  // 遍历接收的目标 MAC 地址（可以有多个地址）
        flag = 2;
        for (j = 0; j < 6; j++) {  // 遍历每个目标 MAC 地址的 6 字节
            if (ethernet_protocol->ether_dhost[j] == accept_dest_mac[i][j])  // 如果匹配
                continue;  // 继续检查下一个字节
            else {  // 如果某个字节不匹配
                flag = i;  // 标记不匹配的目标 MAC 地址索引
                break;  // 中断内层循环，检查下一个目标 MAC 地址
            }
        }
        if (flag != 2) continue;  // 如果找到了不匹配的目标地址，继续下一次外层循环
        else
            break;  // 如果目标 MAC 地址匹配，跳出外层循环
    }

    // 如果目标MAC地址不匹配，则丢弃该数据包
    if (flag != 2) {  // 如果没有找到匹配的目标地址
        return;  // 退出回调函数，丢弃当前数据包
    }

    // 如果是广播包
    if (i == 0) {  // 如果目标地址是广播地址（accept_dest_mac[0]）
        printf("It's broadcasted.\n");  // 打印信息，表示这是一个广播包
    }

    // 检查数据是否发生变化（进行CRC32校验）
    u_int32_t crc = calculate_crc((u_int8_t*)(packet_content + sizeof(ethernet_header)), len - 4 - sizeof(ethernet_header));  // 计算数据部分的 CRC32 校验
    if (crc != *((u_int32_t*)(packet_content + len - 4))) {  // 如果计算的 CRC 不匹配包尾的 CRC 值
        printf("The data has been changed.\n");  // 打印信息，表示数据已经改变
        return;  // 丢弃该数据包
    }

    // 打印捕获的信息
    printf("----------------------------\n");
    printf("capture %d packet\n", packet_number);  // 打印捕获包的编号
    printf("capture time: %d\n", packet_header->ts.tv_sec);  // 捕获时间（秒）
    printf("packet length: %d\n", packet_header->len);      // 数据包长度
    printf("-----Ethernet protocol-------\n");

    ethernet_type = ethernet_protocol->ether_type;  // 获取以太网类型，标识上层协议（如 IPV4、ARP 等）
    printf("Ethernet type: %04x\n", ethernet_type);  // 打印以太网类型（16 进制）

    // 根据以太网类型判断上层协议
    switch (ethernet_type) {
        case 0x0800: printf("Upper layer protocol: IPV4\n"); break;  // 如果是 0x0800，表示上层是 IPv4
        case 0x0806: printf("Upper layer protocol: ARP\n"); break;  // 如果是 0x0806，表示上层是 ARP
        case 0x8035: printf("Upper layer protocol: RARP\n"); break;  // 如果是 0x8035，表示上层是 RARP
        case 0x814c: printf("Upper layer protocol: SNMP\n"); break;  // 如果是 0x814c，表示上层是 SNMP
        case 0x8137: printf("Upper layer protocol: IPX\n"); break;  // 如果是 0x8137，表示上层是 IPX
        case 0x86dd: printf("Upper layer protocol: IPV6\n"); break;  // 如果是 0x86dd，表示上层是 IPV6
        case 0x880b: printf("Upper layer protocol: PPP\n"); break;  // 如果是 0x880b，表示上层是 PPP
        default: break;  // 如果没有匹配的类型，不做处理
    }

    // 打印源MAC地址和目标MAC地址
    mac_string = ethernet_protocol->ether_shost;  // 获取源 MAC 地址
    printf("MAC source address: %02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3),
           *(mac_string + 4), *(mac_string + 5));  // 打印源 MAC 地址（格式：xx:xx:xx:xx:xx:xx）
    mac_string = ethernet_protocol->ether_dhost;  // 获取目标 MAC 地址
    printf("MAC destination address: %02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2),
           *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));  // 打印目标 MAC 地址（格式：xx:xx:xx:xx:xx:xx）

    // 打印数据包的内容（上层协议的数据）
    for (u_int8_t *p = (u_int8_t*)(packet_content + sizeof(ethernet_header)); p != (u_int8_t*)(packet_content + packet_header->len - 4); p++) {  // 遍历数据部分
        printf("%c", *p);  // 按字符输出数据
    }
    printf("\n");

    printf("----------------------\n");
    packet_number++;  // 包号自增
}


int main() {
    generate_crc32_table();  // 生成CRC32查找表

    pcap_if_t *all_adapters;  // 网络适配器列表
    pcap_if_t *adapter;       // 当前适配器
    pcap_t *adapter_handle;   // 网络适配器的句柄
    char error_buffer[PCAP_ERRBUF_SIZE];  // 错误缓冲区

    // 获取所有网络适配器
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &all_adapters, error_buffer) == -1) {
        fprintf(stderr, "Error in findalldevs_ex function: %s\n", error_buffer);
        return -1;  // 如果获取失败，返回-1
    }

    // 如果没有找到适配器，则提示用户
    if (all_adapters == NULL) {
        printf("\nNo adapters found! Make sure WinPcap is installed!!!\n");
        return 0;
    }

    // 打印所有可用的适配器信息
    int id = 1;
    for (adapter = all_adapters; adapter != NULL; adapter = adapter->next) {
        printf("\n%d.%s\n", id++, adapter->name);
        printf("--- %s\n", adapter->description);
    }
    printf("\n");

    // 用户选择一个适配器
    int adapter_id;
    printf("Enter the adapter id between 1 and %d: ", id - 1);
    scanf("%d", &adapter_id);  // 用户输入适配器ID
    if (adapter_id<1 || adapter_id>id - 1) {
        printf("\n Adapter id out of range.\n");
        pcap_freealldevs(all_adapters);
        return -1;  // 如果输入无效，返回-1
    }

    // 根据选择的适配器ID获取具体的适配器
    adapter = all_adapters;
    for (id = 1; id < adapter_id; id++) {
        adapter = adapter->next;
    }

    // 打开选定的适配器
    adapter_handle = pcap_open(adapter->name, 65535, PCAP_OPENFLAG_PROMISCUOUS, 5, NULL, error_buffer);
		//65535: 这是捕获的最大数据包大小，单位是字节。65535 是以太网帧的最大长度，意味着该适配器可以捕获完整的以太网帧数据。
		//表示以混杂模式打开适配器。在混杂模式下，网络适配器将接收所有通过该适配器的数据包，而不仅仅是发送给它的数据包。
		//5: 这是一个超时参数，单位是毫秒，表示打开适配器的超时时间。
		//NULL: 这是一个指向 pcap_rmtauth 结构体的指针，通常用于远程抓包的身份验证。在本例中，使用 NULL 表示不需要身份验证。
    if (adapter_handle == NULL) {
        fprintf(stderr, "\n Unable to open adapter: %s\n", adapter->name);
        pcap_freealldevs(all_adapters);
        return -1;  // 如果打开失败，返回-1
    }

    // 捕获数据包并调用回调函数处理
    pcap_loop(adapter_handle, NULL, ethernet_protocol_packet_callback, NULL);
    pcap_close(adapter_handle);  // 关闭适配器句柄
    pcap_freealldevs(all_adapters);  // 释放所有适配器资源
    return 0;  // 程序结束
}
