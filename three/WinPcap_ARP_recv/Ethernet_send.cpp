#include "Ethernet_send.h"
#include "Resource.h"
#include "Ethernet_recv.h"

u_int32_t size_of_packet = 0;
u_int8_t buffer[MAX_SIZE];
extern pcap_t *handle;
extern u_int8_t local_mac[6];
extern int ethernet_upper_len;

void load_ethernet_header(u_int8_t *destination_mac, u_int16_t ethernet_type)
{
	struct ethernet_header *hdr = (struct ethernet_header *)buffer;
	int i;
	for (i = 0; i < 6; i++)
	{
		hdr->destination_mac[i] = destination_mac[i];
		hdr->source_mac[i] = local_mac[i];
	}

	hdr->ethernet_type = htons(ethernet_type);
	size_of_packet += sizeof(struct ethernet_header);
}


void load_ethernet_data(u_int8_t *buffer, u_int8_t *upper_buffer, int len)
{
	if (len > 1500)
	{
		printf("IP buffer is too large. So we stop the procedure.");
		return;
	}

	int i;
	for (i = 0; i < len; i++)
	{
		*(buffer + i) = *(upper_buffer + i);
	}

	//add a serial 0 at the end
	while (len < 46)
	{
		*(buffer + len) = 0;
		len++;
	}

	u_int32_t crc = calculate_crc(buffer, len);

	*(u_int32_t *)(buffer + len) = crc;
	size_of_packet += len + 4;
}

int ethernet_send_packet(u_int8_t *upper_buffer, u_int8_t *destination_mac, u_int16_t ethernet_type)
{
	load_ethernet_header(destination_mac, ethernet_type);
	load_ethernet_data(buffer + sizeof(struct ethernet_header), upper_buffer, ethernet_upper_len);

	if (pcap_sendpacket(handle, buffer, size_of_packet) != 0)
	{
		printf("Sending Failed!!!\n");
		return -1;
	}
	else
	{
		printf("Sending Succeed...\n");
		return 1;
	}
}