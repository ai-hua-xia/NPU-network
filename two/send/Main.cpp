#include "Network_IPV4_send.h"
#include "Resource.h"

u_int8_t ip_buffer[MAX_SIZE]; // 存储IPv4数据包的内容

int main()
{
	// 查找可用的网络设备
	pcap_if_t *alldevs, *device;
	char error_buffer[PCAP_ERRBUF_SIZE];

	// 获取所有可用的网络设备
	if (pcap_findalldevs(&alldevs, error_buffer) == -1)
	{
		printf("Error finding devices: %s\n", error_buffer);
		return -1;
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
		return -1;
	}

	// 根据用户选择的编号找到对应的设备
	device = alldevs; // 从头开始
	for (int i = 1; i < choice; i++)
	{
		device = device->next; // 迭代直到找到用户选择的设备
	}

	// 打开用户选择的网卡进行数据包捕获
	pcap_t *handle;
	handle = pcap_open_live(device->name, 65536, 1, 1000, error_buffer); // 65536 为数据包最大大小
	if (handle == NULL)
	{
		printf("Open adapter failed: %s\n", error_buffer);
		pcap_freealldevs(alldevs);
		return -1;
	}

	// 打开成功，进行后续操作
	printf("Successfully opened device: %s\n", device->name);

	// open file
	FILE *fp;
	fp = fopen("data_send.txt", "rb");
	network_ipv4_send(ip_buffer, fp, handle);

	fclose(fp);
	pcap_close(handle);
	return 0;
}