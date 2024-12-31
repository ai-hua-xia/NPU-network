#include"Header_Include.h"

struct arp_pkt
{
	u_int16_t hardware_type;
	u_int16_t protocol_type;
	u_int8_t hardware_addr_length;
	u_int8_t protocol_addr_length;
	u_int16_t op_code;
	u_int8_t source_mac[6];
	u_int8_t source_ip[4];
	u_int8_t destination_mac[6]; //request the mac addr
	u_int8_t destination_ip[4];
};


void load_arp_packet(u_int8_t *destination_ip);
/*
if the needer mac addr is not in arp_table, so request
*/
void network_arp_send(u_int8_t *destination_ip, u_int8_t *ethernet_dest_mac);

