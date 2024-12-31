#include"Header_Include.h"

struct ip_header
{
	u_int8_t version_hdrlen;// default IP version: ipv4, header_length: 60bytes
	u_int8_t type_of_service;//
	u_int16_t total_length;//
	u_int16_t id;			//identification
	u_int16_t fragment_offset;//packet maybe need to be fraged. 
	u_int8_t time_to_live;
	u_int8_t upper_protocol_type;
	u_int16_t check_sum;

	u_int8_t source_ip[4];   //this is a structure equval to u_int32_t, but can be used in windows socket api
	u_int8_t destination_ip[4];

	u_int8_t optional[40];//40 bytes is optional

};


u_int16_t calculate_check_sum(ip_header *ip_hdr, int len);
//there is some bits that value is 0, so len as a parameter join the function
int network_ipv4_recv(u_int8_t *ip_buffer);

int is_accept_ip_packet(struct ip_header *ip_hdr);

void icmp_recv(u_int8_t *ip_buffer);
void send_icmp_echo_reply(u_int8_t *ip_buffer, int ip_len, struct icmp_header *icmp_request);
u_int16_t calculate_checksum(struct icmp_header *icmp_hdr, int len);
int ethernet_send_packet(u_int8_t *upper_buffer, u_int8_t *destination_mac, u_int16_t ethernet_type);

// ICMP 头部结构
struct icmp_header {
    u_int8_t type;
    u_int8_t code;
    u_int16_t checksum;
    u_int16_t id;
    u_int16_t sequence;
};


