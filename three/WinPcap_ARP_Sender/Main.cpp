#include "ARP_Cache_Table.h"
#include "Resource.h"
#include "Ethernet.h"
#include "Network_ARP_send.h"
#include "Network_IPV4_send.h"
#include "Header_Include.h"

u_int8_t ip_buffer[MAX_SIZE];
extern pcap_t *handle;

int main()
{
	// initial the arp_table
	init_arp_table();
	output_arp_table();

	open_device();
	FILE *fp;
	fp = fopen("data_send.txt", "rb");

	network_ipv4_send(ip_buffer, fp);

	fclose(fp);
	close_device();
	system("pause");
	return 0;
}