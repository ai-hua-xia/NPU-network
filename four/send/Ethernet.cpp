#include "Ethernet.h"
#include "Resource.h"
#include"Network_IPV4_send.h"
pcap_if_t *alldevs; // 存储所有设备的指针
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
	//open device
	char *device;
	char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_if_t *d; // 遍历设备列表的指针
    int inum; // 接口编号
    int i = 0; // 接口计数器

    // 获取所有设备
    if (pcap_findalldevs(&alldevs, error_buffer) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", error_buffer);
        exit(1);
    }

    // 打印所有设备
    for(d = alldevs; d; d = d->next) {
        printf("%d. %s [%s]\n", ++i, d->name, d->description ? d->description : "No description");
    }
    if (i == 0) {
        printf("\nNo interfaces found! Make sure WinPcap is installed and working properly.\n");
        pcap_freealldevs(alldevs);
        return ;
    }

    // 用户选择设备
    printf("Enter the interface number (1-%d): ", i);
    scanf("%d", &inum);

    // 验证用户输入
    if (inum < 1 || inum > i) {
        printf("\nInterface number out of range.\n");
        pcap_freealldevs(alldevs);
        return ;
    }
     // 根据用户选择找到对应的设备
    for (d = alldevs, i = 1; i < inum ; d = d->next, i++);
    
    device = d->name;
	handle = pcap_open_live(device, 65536, 1, 1000, error_buffer);
	
	if (handle == NULL) {
        fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", device);
        pcap_freealldevs(alldevs);
        return ;
    }

	generate_crc32_table();
}
void close_device()
{
	pcap_close(handle);
	pcap_freealldevs(alldevs);
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
