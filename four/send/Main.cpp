#include "ARP_Cache_Table.h"
#include "Resource.h"
#include "Ethernet.h"
#include "Network_ARP_send.h"
#include "Network_IPV4_send.h"
#include "Transport_UDP_send.h"
#include "Header_Include.h"
extern pcap_t *handle;
u_int8_t ip_buffer[MAX_SIZE];
extern u_int8_t broadcast_mac[6];
extern u_int8_t target_ip[4];

int main()
{
	//initial the arp_table
	init_arp_table();
	u_int8_t dest_mac[6] = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
	u_int8_t dest_ip[4] = { 10, 10, 10, 10 };
	struct arp_node *dest_node = make_arp_node(dest_ip, dest_mac, STATIC_STATE);
    insert_arp_node(dest_node);
    
	output_arp_table();
	
	open_device();

	FILE *fp;
	fp = fopen("data.txt", "rb");

	//network_ipv4_send(ip_buffer, fp, IPPROTO_TCP);
	transport_udp_send(ip_buffer, IPPROTO_UDP);
  send_icmp_echo_request(target_ip, broadcast_mac,IPPROTO_ICMP);
	
	
	fclose(fp);
	close_device();
	system("pause");
	return 0;
}
