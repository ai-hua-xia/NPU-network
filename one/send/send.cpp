#include <stdio.h>
#include <stdlib.h>

#define HAVE_REMOTE
#include <pcap.h>

// 禁用 Visual Studio 编译器中的安全警告
#pragma warning(disable:4996)

// 定义以太网协议类型字段值（IP 协议）
#define ETHERNET_IP 0x0800
// 设置最大数据包大小
#define MAX_SIZE 2048

// 数据包的大小
int size_of_packet = 0;

// CRC32 查找表
u_int32_t crc32_table[256];

// 以太网帧头结构体
struct ethernet_header {
    u_int8_t dest_mac[6];  // 目标 MAC 地址
    u_int8_t src_mac[6];   // 源 MAC 地址
    u_int16_t ethernet_type;  // 以太网类型
};

// 生成 CRC32 查找表
void generate_crc32_table() {
    int i, j;
    u_int32_t crc;
    
    // 生成 CRC32 查找表，用于快速计算 CRC32 校验
    for (i = 0; i < 256; i++) {
        crc = i;  // 初始化 crc 为当前字节的值（i），作为 CRC 的起始值
        for (j = 0; j < 8; j++) {  // 对每个字节的 8 位进行处理
            if (crc & 1)  // 如果 CRC 的最低位是 1
                crc = (crc >> 1) ^ 0xEDB88320;  // 实际上是在对 CRC 进行除法操作
            else
                crc >>= 1;  // 右移
        }
        crc32_table[i] = crc;  // 将计算出的 CRC 值存入表中
    }
}

// 计算 CRC32 校验值
u_int32_t calculate_crc(u_int8_t *buffer, int len) {
    int i;
    u_int32_t crc = 0xffffffff;  // CRC 初始化值为 0xFFFFFFFF
    
    // 遍历数据并计算 CRC32 校验值
    /*
    crc = (crc >> 8)：这里将当前 CRC 值右移 8 位。这个操作相当于将 CRC 的每个字节的影响向下移一位，以便进行下一个字节的计算。
    crc & 0xFF：提取当前 CRC 值的最低 8 位（即当前字节的 CRC）。
    crc32_table[(crc & 0xFF) ^ buffer[i]]：这是查找表的使用步骤。这个操作会根据当前 CRC 的低 8 位与数据字节（buffer[i]）进行异或，得到新的索引，然后使用查找表获取对应的 CRC32 结果。*/
    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ buffer[i]];  //逐字节进行
    }
    
    // 取反得到最终 CRC32 值
    crc ^= 0xffffffff;
    return crc;
}

// 加载以太网帧头
void load_ethernet_header(u_int8_t *buffer) {
    struct ethernet_header *hdr = (struct ethernet_header*)buffer;
    
    // 设置目标 MAC 地址
    hdr->dest_mac[0] = 0x00;
    hdr->dest_mac[1] = 0x09;
    hdr->dest_mac[2] = 0x73;
    hdr->dest_mac[3] = 0x07;
    hdr->dest_mac[4] = 0x74;
    hdr->dest_mac[5] = 0x73;
    // hdr->dest_mac[0] = 0x11;
    // hdr->dest_mac[1] = 0x11;
    // hdr->dest_mac[2] = 0x11;
    // hdr->dest_mac[3] = 0x11;
    // hdr->dest_mac[4] = 0x11;
    // hdr->dest_mac[5] = 0x11;

    // 设置源 MAC 地址
    hdr->src_mac[0] = 0x00;
    hdr->src_mac[1] = 0x09;
    hdr->src_mac[2] = 0x73;
    hdr->src_mac[3] = 0x07;
    hdr->src_mac[4] = 0x73;
    hdr->src_mac[5] = 0xf9;
    
    // 设置以太网类型为 IP 协议
    hdr->ethernet_type = ETHERNET_IP;

    size_of_packet += sizeof(ethernet_header);  // 更新数据包的大小
}

// 加载以太网数据并计算 CRC
int load_ethernet_data(u_int8_t *buffer, FILE *fp) {
    int size_of_data = 0;
    char tmp[MAX_SIZE], ch;
    
    // 从文件读取数据到临时数组 tmp
    while ((ch = fgetc(fp)) != EOF) {
        tmp[size_of_data] = ch;
        size_of_data++;
    }
    
    // 数据长度必须在 46 到 1500 字节之间（以太网最小帧长和最大帧长）
    if (size_of_data < 46 || size_of_data > 1500) {
        printf("Size of data is not satisfied with condition!!!\n");
        return -1;  // 如果数据长度不符合要求，则返回错误
    }

    // 计算数据的 CRC32 校验
    u_int32_t crc = calculate_crc((u_int8_t*)tmp, size_of_data);
    int i;
    
    // 将数据复制到 buffer 中
    for (i = 0; i < size_of_data; i++) {
        *(buffer + i) = tmp[i];
    }
    
    // 将 CRC 值添加到数据末尾
    *(u_int32_t*)(buffer + i) = crc;
    size_of_packet += size_of_data + 4;  // 更新数据包大小，加上 CRC 长度（4 字节）
    return 1;  // 返回成功
}

// 主函数
// 主函数
int main() {
    u_int8_t buffer[MAX_SIZE];  // 定义一个缓冲区用于存放数据包
    generate_crc32_table();  // 生成 CRC32 查找表

    // 初始化数据包大小
    size_of_packet = 0;
    
    // 打开文件并加载数据
    FILE *fp = fopen("data.txt", "r");
    if (load_ethernet_data(buffer + sizeof(ethernet_header), fp) == -1) {  //数据存放在 buffer 的以太网头部后面的位置
        return -1;  // 如果加载数据失败，则退出
    }
    load_ethernet_header(buffer);  // 加载以太网帧头

    // 列出可用的网络设备
    pcap_if_t *alldevs, *device;
    char error_buffer[PCAP_ERRBUF_SIZE];
    
    // 获取系统中的网络设备
    if (pcap_findalldevs(&alldevs, error_buffer) == -1) {
        printf("Error finding devices: %s\n", error_buffer);
        return -1;
    }

    // 打印所有可用的设备列表，以接收端的格式为参考
    printf("Available adapters:\n");
    int id = 1;
    for (device = alldevs; device != NULL; device = device->next) {
        printf("\n%d. %s\n", id++, device->name);  // 打印设备索引和设备名称
        printf("--- %s\n", device->description);  // 打印设备描述
    }
    printf("\n");

    int choice;
    printf("Enter the adapter id (1-%d): ", id - 1);
    scanf("%d", &choice);  // 用户选择适配器

    // 检查用户选择是否有效
    if (choice < 1 || choice >= id) {
        printf("Invalid choice.\n");
        pcap_freealldevs(alldevs);
        return -1;
    }

    // 根据用户选择找到设备
    device = alldevs;  // 将 device 设置为设备链表的第一个元素
    for (int i = 1; i < choice; i++) {
        device = device->next;  //找到对应 choice 索引的设备
    }

    // 打开选定的网络适配器进行数据包发送
    pcap_t *handle;
    handle = pcap_open_live(device->name, size_of_packet, PCAP_OPENFLAG_PROMISCUOUS, 1, error_buffer); //1 表示打开时不进行超时限制
    if (handle == NULL) {
        printf("Open adapter failed: %s\n", error_buffer);
        pcap_freealldevs(alldevs);
        return -1;
    }

    // 循环发送数据包 20 次
    int i = 20;
    while (i--) {
        pcap_sendpacket(handle, (const u_char*)buffer, size_of_packet);  // 发送数据包
    }

    // 关闭设备和释放资源
    pcap_close(handle);
    pcap_freealldevs(alldevs);
    return 0;
}

