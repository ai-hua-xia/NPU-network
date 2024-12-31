#include "Header_Include.h"

struct ethernet_header
{
	u_int8_t destination_mac[6];
	u_int8_t source_mac[6];
	u_int16_t ethernet_type;
};

//generate crc table
void generate_crc32_table();
//calculate crc
u_int32_t calculate_crc(u_int8_t *buffer, int len);

void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content);
int is_accept_ethernet_packet(u_int8_t *packet_content, int len);

void open_device();
void close_device();





