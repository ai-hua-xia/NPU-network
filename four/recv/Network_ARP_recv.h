#include "Network_ARP_send.h"

int is_accept_arp_packet(struct arp_pkt *arp_packet);
u_int8_t* network_arp_recv(u_int8_t *arp_buffer);

void output(struct arp_pkt *arp_packet);