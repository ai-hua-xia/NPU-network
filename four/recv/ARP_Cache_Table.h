#include "Header_Include.h"

struct arp_node
{
	u_int8_t ip[4];
	u_int8_t mac[6];
	u_int8_t state;
	struct arp_node *next;
};

struct arp_table_header
{
	arp_node *queue;
	arp_node *head;
	arp_node *tail;
};

//add the local ip to mac
void init_arp_table();

struct arp_node* make_arp_node(u_int8_t *ip, u_int8_t *mac, int state);

void insert_arp_node(struct arp_node *node);

int delete_arp_node(struct arp_node *node);

int update_arp_node(struct arp_node *node);

/*
if ip existed, return mac
else  return NULL
*/
u_int8_t* is_existed_ip(u_int8_t *destination_ip);

//check the queue;
void output_arp_table();