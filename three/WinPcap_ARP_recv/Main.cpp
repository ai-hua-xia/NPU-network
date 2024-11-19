#include "Header_Include.h"
#include "Ethernet_recv.h"
#include "Ethernet_send.h"
#include "ARP_Cache_Table.h"


extern pcap_t *handle;
int main()
{
	init_arp_table();
	output_arp_table();

	open_device();

	pcap_loop(handle, NULL, ethernet_protocol_packet_callback, NULL);

	close_device();

	return 0;
}