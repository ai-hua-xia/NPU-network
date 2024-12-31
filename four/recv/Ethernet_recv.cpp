#include "Ethernet_recv.h"
#include "Network_IPV4_recv.h"
#include "Network_ARP_recv.h"

pcap_if_t *alldevs; // 存储所有设备的指针
u_int32_t crc32_table[256];
u_int32_t packet_number = 1;
extern u_int8_t local_mac[6];
extern pcap_t *handle;

//generate table
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

int is_accept_ethernet_packet(u_int8_t *packet_content, int len)
{
	struct ethernet_header *ethernet_hdr = (struct ethernet_header *)packet_content;
	int i;
	int flag = 0;
	for (i = 0; i < 6; i++)
	{
		if (ethernet_hdr->destination_mac[i] != 0xff)break;
	}

	if (i == 6)
	{
		flag = 1;
		printf("It's broadcast packet.\n");
	}

	for (i = 0; i < 6; i++)
	{
		if (ethernet_hdr->destination_mac[i] != local_mac[i])break;
	}

	if (i == 6)
	{
		flag = 1;
		printf("It's sended to my pc.\n");
	}
	if (!flag)
		return 0;

	//generate_crc32_table();
	//crc match
	u_int32_t crc = calculate_crc((u_int8_t *)(packet_content + sizeof(ethernet_header)), len - 4 - sizeof(ethernet_header));
	if (crc != *((u_int32_t *)(packet_content + len - 4)))
	{
		printf("The data has changed.\n");
		return 0;
	}
	return 1;
}


void output_mac(u_int8_t mac[6])
{
	for (int i = 0; i < 6; i++)
	{
		if (i)printf("-");
		printf("%02x", mac[i]);
	}
	printf("\n");
}

void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
	int len = packet_header->len;
	if (!is_accept_ethernet_packet((u_int8_t *)packet_content, len))
	{
		return;
	}

	struct ethernet_header *ethernet_hdr = (struct ethernet_header *)packet_content;
	u_int16_t ethernet_type = ntohs(ethernet_hdr->ethernet_type);
	

	printf("Capture %d packet\n", packet_number++);
	printf("Capture time: %d %d\n", packet_header->ts.tv_sec, packet_header->ts.tv_usec);
	//output_time(packet_header->ts.tv_sec);
	printf("Packet length: %d\n", packet_header->len);

	printf("--------------------------Ethernet Protocol------------------------\n");
	printf("Ethernet type:  %04x\n", ethernet_type);
	printf("MAC source address: ");
	output_mac(ethernet_hdr->source_mac);
	printf("MAC destination address: ");
	output_mac(ethernet_hdr->destination_mac);

	u_int8_t *upper_buffer = (u_int8_t *)(packet_content + sizeof(ethernet_header));

	switch (ethernet_type)
	{
	case 0x0800:
		printf("Upper layer protocol: IPV4\n");
		network_ipv4_recv(upper_buffer);
		break;
	case 0x0806:
		printf("Upper layer protocol: ARP\n");
		network_arp_recv(upper_buffer);
		break;
	case 0x8035:
		printf("Upper layer protocol: RARP\n");
		//network_rarp_recv();
		break;
	case 0x814c:
		printf("Upper layer protocol: SNMP\n");
		//network_snmp_recv();
		break;
	case 0x8137:
		printf("Upper layer protocol: IPX(Internet Packet Exchange)\n");
		//network_ipx_recv();
		break;
	case 0x86DD:
		printf("Upper layer protocol: IPV6\n");
		//network_ipv6_recv();
		break;
	case 0x880B:
		printf("Upper layer protocol: PPP\n");
		//network_ppp_recv();
		break;
	default:break;
	}

	printf("-------------------End of Ethernet Protocol----------------\n");
}

void open_device()
{
	generate_crc32_table();
	
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
	/*char *device;
	char error_buffer[PCAP_ERRBUF_SIZE];

	device = pcap_lookupdev(error_buffer);

	handle = pcap_open_live(device, 65536, 1, 1000, error_buffer);*/
}

void close_device()
{
	pcap_close(handle);
	pcap_freealldevs(alldevs);
}
