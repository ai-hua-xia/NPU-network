#include "Ethernet.h"
#include "Resource.h"
u_int32_t crc32_table[256] = { 0 };
extern u_int32_t size_of_packet = 0;


void generate_crc32_table()
{
	int i, j;
	u_int32_t crc;
	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
		crc32_table[i] = crc;
	}
}

u_int32_t calculate_crc(u_int8_t *buffer, int len)
{
	int i;
	u_int32_t crc;
	crc = 0xffffffff;
	for (i = 0; i < len; i++)
	{
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ buffer[i]];
	}
	crc ^= 0xffffffff;
	return crc;
}


void load_ethernet_header(u_int8_t *buffer)
{
	struct ethernet_header *hdr = (struct ethernet_header*)buffer;
	size_of_packet = 0;
	// add destination mac address
	hdr->destination_mac[0] = 0x11;
	hdr->destination_mac[1] = 0x11;
	hdr->destination_mac[2] = 0x11;
	hdr->destination_mac[3] = 0x11;
	hdr->destination_mac[4] = 0x11;
	hdr->destination_mac[5] = 0x11;

	//add source mac address
	hdr->source_mac[0] = 0x44;
	hdr->source_mac[1] = 0x37;
	hdr->source_mac[2] = 0xE6;
	hdr->source_mac[3] = 0x89;
	hdr->source_mac[4] = 0xCB;
	hdr->source_mac[5] = 0x7F;

	// add source typy
	hdr->ethernet_type = htons(ETHERNET_IP);

	// caculate the size of packet now
	size_of_packet += sizeof(ethernet_header);
}

int load_ethernet_data(u_int8_t *buffer, u_int8_t *ip_buffer,int len)
{
	if (len > 1500)
	{
		printf("IP buffer is too large. So we stop the procedure.");
		return -1;
	}

	int i;
	for (i = 0; i < len; i++)
	{
		*(buffer + i) = *(ip_buffer + i);
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
	return 1;
}

int ethernet_send_packet(u_int8_t *buffer,pcap_t *handle)
{
	if (pcap_sendpacket(handle, (const u_char *)buffer, size_of_packet) != 0)
	{
		printf("Sending failed..\n");
		return -1;
	}
	else
	{
		printf("Sending Succeed..\n");
		return 1;
	}
}
