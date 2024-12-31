#include"ARP_Cache_Table.h"
#include "Resource.h"

extern u_int8_t local_mac[6];
extern u_int8_t local_ip[4];
extern u_int8_t gateway_ip[4];
extern u_int8_t netmask[4];
extern u_int8_t dns_server_ip[4];
extern u_int8_t dhcp_server_ip[4];

arp_table_header arp_table;

struct arp_node* make_arp_node(u_int8_t *ip, u_int8_t *mac, int state)
{
	int i;
	struct arp_node *node = (struct arp_node *)malloc(sizeof(struct arp_node));
	for (i = 0; i < 4; i++)
	{
		node->ip[i] = ip[i];
	}

	for (i = 0; i < 6; i++)
	{
		node->mac[i] = mac[i];
	}
	node->state = state;
	node->next = NULL;
	return node;
}

void init_arp_table()
{
	struct arp_node *node;
	node = make_arp_node(local_ip, local_mac, STATIC_STATE);

	arp_table.queue = node;
	arp_table.head = node;
	arp_table.tail = node;
}

void insert_arp_node(struct arp_node *node)
{
	if (!is_existed_ip(node->ip))
	{
		arp_table.tail->next = node;
		arp_table.tail = node;
	}
}

int delete_arp_node(struct arp_node *node)
{
	struct arp_node *pre = arp_table.head;
	struct arp_node *p = pre->next;
	int flag = 1;
	while (p != NULL)
	{
		int i;
		flag = 1;
		for (i = 0; i < 4; i++)
		{
			if (node->ip[i] != p->ip[i])
			{
				flag = 0;
				break;
			}
		}

		for (i = 0; i < 6; i++)
		{
			if (node->mac[i] != p->mac[i])
			{
				flag = 0;
				break;
			}
		}

		if (flag)
		{
			pre->next = p->next;
			free(p);
			break;
		}
		
		pre = p;
		p = p->next;
	}
	if (flag)
	{
		printf("delete arp node succeed!!!\n");
		return 1;
	}
	else
	{
		printf("Failed delete\n");
		return 0;
	}
}

u_int8_t* is_existed_ip(u_int8_t *destination_ip)
{
	struct arp_node *p = arp_table.head;
	int flag = 1;
	while (p != NULL)
	{
		int i;
		flag = 1;
		for (i = 0; i < 4; i++)
		{
			if (p->ip[i] != destination_ip[i])
			{
				flag = 0;
				break;
			}
		}

		if (flag)
		{
			return p->mac;
		}
		p = p->next;
	}
	return NULL;
}

int update_arp_node(struct arp_node *node)
{
	u_int8_t *mac = is_existed_ip(node->ip);
	if (mac)
	{
		int i;
		for (i = 0; i < 6; i++)
		{
			mac[i] = node->mac[i];
		}
		printf("Update succeed.\n");
		return 1;
	}
	else
	{
		printf("Update failed.\n");
		return 0;
	}
}

void output_arp_table()
{
	struct arp_node *p = arp_table.head;
	while (p != NULL)
	{
		int i;
		printf("IP:");
		for (i = 0; i < 4; i++)
		{
			if (i)printf(".");
			printf("%d", p->ip[i]);
		}
		printf("\tMAC:");
		for (i = 0; i < 6; i++)
		{
			if (i)printf("-");
			printf("%02x", p->mac[i]);
		}
		printf("\n");

		p = p->next;
	}

}

