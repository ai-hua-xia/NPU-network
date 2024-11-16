#include "Network_IPV4_send.h"
#include "Resource.h"

u_int8_t buffer[MAX_SIZE];
u_int16_t ip_packet_id = 0; // as flag in ip_header->id
u_int32_t ip_size_of_packet = 0;

// 算IPv4数据包头部的校验和。校验和是一个16位的数字
u_int16_t calculate_check_sum(ip_header *ip_hdr, int len)
{
	int sum = 0, tmp = len;							// 保存传入的len值（用于处理单字节的情况）
	u_int16_t *p = (u_int16_t *)ip_hdr; // 指向IPv4头部的指针
	while (len > 1)
	{
		sum += *p;
		len -= 2; // 每次读取16位数据（2字节）
		p++;
	}

	// 如果len是奇数，则剩下的1字节需要单独处理
	if (len)
	{
		sum += *((u_int8_t *)ip_hdr + tmp - 1); // 将IPv4头部的最后一个字节（tmp - 1位置）加到sum中
	}

	// 将计算出的32位和折叠成16位
	while (sum >> 16) // 检查是否需要折叠（即是否有溢出）
	{
		sum = (sum & 0xffff) + (sum >> 16); // 将高16位的值加到低16位，直到没有溢出为止
	}

	return ~sum;
}

// 初始化一个 IPv4 数据包的头部
void load_ip_header(u_int8_t *ip_buffer)
{
	struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
	// 全局变量，记录 IP 数据包的总大小，初始化为 0
	ip_size_of_packet = 0;
	// initial the ip header
	ip_hdr->version_hdrlen = 0x4f;						 // 4 表示 IPv4（IPv4 的版本是 4）,f表示 IPv4 头部的长度为 60 字节
	ip_hdr->type_of_service = 0xfe;						 /*111 1 1110: first 3 bits: priority level,
															 then 1 bit: delay, 1 bit: throughput, 1 bit: reliability
															 1 bit: routing cost, 1 bit: unused
															 */
	ip_hdr->total_length = 0;									 // wait for data length, 0 for now
	ip_hdr->id = ip_packet_id;								 // identification
	ip_hdr->fragment_offset = 0x0000;					 /*0 0 0 0 00...00: first 3 bits is flag: 1 bit: 0 the last fragment,
														 1 more fragmet. 1 bit: 0 allow fragment, 1 don't fragment. 1 bit: unused
														 the last 12 bits is offset
														 */
	ip_hdr->time_to_live = 64;								 // default 1000ms
	ip_hdr->upper_protocol_type = IPPROTO_TCP; // default upper protocol is tcp
	ip_hdr->check_sum = 0;										 // initial zero

	ip_hdr->source_ip.s_addr = inet_addr("10.13.80.43"); // convert ip string to a unsigned long
	ip_hdr->destination_ip.s_addr = inet_addr("255.255.255.255");

	// initial check_sum is associate with offset. so in the data we need to calculate check_sum
	ip_size_of_packet += sizeof(ip_header);
}

// 数据填充到 IP 数据包的有效负载部分
void load_ip_data(u_int8_t *ip_buffer, FILE *fp, int len)
{
	int i = 0;
	char ch;
	while (i < len && (ch = fgetc(fp)) != EOF)
	{
		*(ip_buffer + i) = ch;
		i++;
	}
	ip_size_of_packet += len;
}

// 将文件内容分片并发送作为多个IPv4数据包
int network_ipv4_send(u_int8_t *ip_buffer, FILE *fp, pcap_t *handle)
{
	// get the size of file(获取文件大小)
	int file_len;
	fseek(fp, 0, SEEK_END); // 将文件指针移动到文件的末尾
	file_len = ftell(fp);		// 返回文件指针当前位置
	rewind(fp);							// 将文件指针移回文件开头

	// get how many fragments(计算分片数量)
	int number_of_fragment = (int)ceil(file_len * 1.0 / MAX_IP_PACKET_SIZE);
	u_int16_t offset = 0;			 // 用于跟踪当前已经处理的字节位置
	int ip_data_len;					 // 每个分片的数据部分长度（不包括IPv4头部）
	u_int16_t fragment_offset; // 分片的偏移量
	while (number_of_fragment)
	{
		load_ip_header(ip_buffer); // 填充IPv4头部
		struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
		if (number_of_fragment == 1)
		{
			fragment_offset = 0x0000;				 // 16bits
			ip_data_len = file_len - offset; // 最后一个分片的长度
		}
		else
		{
			fragment_offset = 0x2000; // allow the next fragment
			ip_data_len = MAX_IP_PACKET_SIZE;
		}

		fragment_offset |= ((offset / 8) & 0x0fff);
		ip_hdr->fragment_offset = htons(fragment_offset);

		// printf("%04x\n", ip_hdr->fragment_offset);
		ip_hdr->total_length = htons(ip_data_len + sizeof(ip_header)); // 更新IPv4头部的总长度
		ip_hdr->check_sum = calculate_check_sum(ip_hdr, 60);					 // 计算并更新IPv4头部的校验和
		// printf("%04x\n", ip_hdr->check_sum);

		// 从文件fp中加载数据到ip_buffer，并确保加载的数据部分长度为ip_data_len
		load_ip_data(ip_buffer + sizeof(ip_header), fp, ip_data_len);

		load_ethernet_header(buffer); // 构建以太网头部
		// 将IP数据（包括IPv4头和数据部分）加载到以太网数据部分
		load_ethernet_data(buffer + sizeof(ethernet_header), ip_buffer, ip_size_of_packet);
		// while (1)
		ethernet_send_packet(buffer, handle);

		offset += MAX_IP_PACKET_SIZE;
		number_of_fragment--;
	}

	// auto increase one
	ip_packet_id++;

	return 1;
}
