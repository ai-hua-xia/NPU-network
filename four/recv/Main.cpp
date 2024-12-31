#include "Header_Include.h"
#include "Ethernet_recv.h"
#include "Ethernet_send.h"
#include "ARP_Cache_Table.h"
#include "Transport_UDP.h"
extern pcap_t *handle;

int main()
{
	init_arp_table();
	output_arp_table();

	open_device();
	transport_udp_recv();
	pcap_loop(handle, -1, ethernet_protocol_packet_callback, NULL);
	
	
	close_device();
	system("pause");
	return 0;
}
