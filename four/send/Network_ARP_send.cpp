#include"Network_ARP_send.h"
#include"Ethernet.h"
#include"Resource.h"

extern u_int8_t local_mac[6];
extern u_int8_t local_ip[4];
extern int ethernet_upper_len;

u_int8_t arp_buffer[MAX_SIZE];


void load_arp_packet(u_int8_t *destination_ip)
{
	struct arp_pkt *arp_packet = (struct arp_pkt *)(arp_buffer);
	arp_packet->hardware_type = htons(ARP_HARDWARE);
	arp_packet->protocol_type = htons(ETHERNET_IP);
	arp_packet->hardware_addr_length = 6;
	arp_packet->protocol_addr_length = 4;
	arp_packet->op_code = htons(ARP_REQUEST);
	int i;
	for (i = 0; i < 6; i++)
	{
		arp_packet->source_mac[i] = local_mac[i];
	}
	for (i = 0; i < 4; i++)
	{
		arp_packet->source_ip[i] = local_ip[i];
	}

	for (i = 0; i < 6; i++)
	{
		arp_packet->destination_mac[i] = 0x00;
	}
	for (i = 0; i < 4; i++)
	{
		arp_packet->destination_ip[i] = destination_ip[i];
	}
}


void network_arp_send(u_int8_t *destination_ip, u_int8_t *ethernet_dest_mac)
{
	load_arp_packet(destination_ip);
	ethernet_upper_len = sizeof(struct arp_pkt);

	ethernet_send_packet(arp_buffer,ethernet_dest_mac, ETHERNET_ARP);
}