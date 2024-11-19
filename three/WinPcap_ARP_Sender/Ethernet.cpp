#include "Ethernet.h"
#include "Resource.h"
#include"Network_IPV4_send.h"

u_int32_t crc32_table[256] = { 0 };
u_int32_t size_of_packet = 0;

u_int8_t buffer[MAX_SIZE];
extern pcap_t *handle;
extern u_int8_t local_mac[6];
extern int ethernet_upper_len;

void generate_crc32_table()
{
	int i, j;
	u_int32_t crc;
	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
		crc32_table[i] = crc;
	}
}

u_int32_t calculate_crc(u_int8_t *buffer, int len)
{
	int i;
	u_int32_t crc;
	crc = 0xffffffff;
	for (i = 0; i < len; i++)
	{
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ buffer[i]];
	}
	crc ^= 0xffffffff;
	return crc;
}


void load_ethernet_header(u_int8_t *destination_mac,u_int16_t ethernet_type)
{
	struct ethernet_header *hdr = (struct ethernet_header *)buffer;
	size_of_packet = 0;
	// add destination mac address
	hdr->destination_mac[0] = destination_mac[0];
	hdr->destination_mac[1] = destination_mac[1];
	hdr->destination_mac[2] = destination_mac[2];
	hdr->destination_mac[3] = destination_mac[3];
	hdr->destination_mac[4] = destination_mac[4];
	hdr->destination_mac[5] = destination_mac[5];

	//add source mac address
	hdr->source_mac[0] = local_mac[0];
	hdr->source_mac[1] = local_mac[1];
	hdr->source_mac[2] = local_mac[2];
	hdr->source_mac[3] = local_mac[3];
	hdr->source_mac[4] = local_mac[4];
	hdr->source_mac[5] = local_mac[5];

	// add source typy
	hdr->ethernet_type = htons(ethernet_type);

	// caculate the size of packet now
	size_of_packet += sizeof(ethernet_header);
}

int load_ethernet_data(u_int8_t *buffer, u_int8_t *upper_buffer, int len)
{
	if (len > 1500)
	{
		printf("IP buffer is too large. So we stop the procedure.");
		return -1;
	}

	int i;
	for (i = 0; i < len; i++)
	{
		*(buffer + i) = *(upper_buffer + i);
	}

	//add a serial 0 at the end
	while (len < 46)
	{
		*(buffer + len) = 0;
		len++;
	}
    
    //generate_crc32_table();
	u_int32_t crc = calculate_crc(buffer, len);

	*(u_int32_t *)(buffer + len) = crc;
	size_of_packet += len + 4;
	return 1;
}

int ethernet_send_packet(u_int8_t *upper_buffer,u_int8_t *destination_mac,u_int16_t ethernet_type)
{
	load_ethernet_header(destination_mac, ethernet_type);
	load_ethernet_data(buffer + sizeof(struct ethernet_header), upper_buffer, ethernet_upper_len);

	if (pcap_sendpacket(handle, (const u_char *)buffer, size_of_packet) != 0)
	{
		printf("Sending failed..\n");
		return -1;
	}
	else
	{
		printf("Sending Succeed..\n");
		return 1;
	}
}


void open_device()
{
	// char *device;
	// char error_buffer[PCAP_ERRBUF_SIZE];

	// device = pcap_lookupdev(error_buffer);
	// handle = pcap_open_live(device, 65536, 1, 1000, error_buffer);
	// 查找可用的网络设备
	pcap_if_t *alldevs, *device;
	char error_buffer[PCAP_ERRBUF_SIZE];

	// 获取所有可用的网络设备
	if (pcap_findalldevs(&alldevs, error_buffer) == -1)
	{
		printf("Error finding devices: %s\n", error_buffer);
	}

	// 显示所有可用设备的列表
	printf("send_Available adapters:\n");
	int id = 1;
	for (device = alldevs; device != NULL; device = device->next)
	{
		printf("\n%d. %s\n", id++, device->name); // 显示设备名称
		printf("--- %s\n", device->description);	// 显示设备描述
	}
	printf("\n");

	// 用户输入选择的网卡编号
	int choice;
	printf("Enter the adapter id (1-%d): ", id - 1);
	scanf("%d", &choice); // 用户选择网卡编号

	// 验证用户输入的编号是否有效
	if (choice < 1 || choice >= id)
	{
		printf("Invalid choice.\n");
		pcap_freealldevs(alldevs);
	}

	// 根据用户选择的编号找到对应的设备
	device = alldevs; // 从头开始
	for (int i = 1; i < choice; i++)
	{
		device = device->next; // 迭代直到找到用户选择的设备
	}

	// 打开用户选择的网卡进行数据包捕获
	handle = pcap_open_live(device->name, 65536, 1, 1000, error_buffer); // 65536 为数据包最大大小
	if (handle == NULL)
	{
		printf("Open adapter failed: %s\n", error_buffer);
		pcap_freealldevs(alldevs);
	}

	// 打开成功，进行后续操作
	printf("Successfully opened device: %s\n", device->name);
	generate_crc32_table();
}
void close_device()
{
	pcap_close(handle);
}

//broadcast and local is acceptable
int is_accept_ethernet_packet(struct ethernet_header *ethernet_hdr)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (ethernet_hdr->destination_mac[i] != 0xff)
			break;
	}
	if (i == 6)
	{
		printf("It's broadcast packet.\n");
		return 1;
	}

	for (i = 0; i < 6; i++)
	{
		if (ethernet_hdr->destination_mac[i] != local_mac[i])
			break;
	}

	if (i == 6)
	{
		printf("It's sended to my pc.\n");
		return 1;
	}
	return 0;
}