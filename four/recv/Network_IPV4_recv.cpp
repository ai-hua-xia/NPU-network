#include "Network_IPV4_recv.h"
#include "Transport_UDP.h"
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0
#define MAX_DATA_SIZE 1000000
u_int16_t ip_id = 0;
u_int16_t i = 0;

u_int8_t data_buffer[MAX_DATA_SIZE];

int previous = 0, current = 0;

extern u_int8_t local_ip[4];

/*
if allow fragment, store to buffer until not allow, then
store to file.
*/

u_int16_t calculate_check_sum(ip_header *ip_hdr, int len)
{
	int sum = 0, tmp = len;
	u_int16_t *p = (u_int16_t*)ip_hdr;
	while (len > 1)
	{
		sum += *p;
		len -= 2;
		p++;
	}

	//len=1 last one byte
	if (len)
	{
		sum += *((u_int8_t*)ip_hdr + tmp - 1);
	}

	//fold 32 bits to 16 bits
	while (sum >> 16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}

	return ~sum;
}


int is_accept_ip_packet(struct ip_header *ip_hdr)
{
	int i;
	int flag = 0;
	for (i = 0; i < 4; i++)
	{
		if (ip_hdr->destination_ip[i] != local_ip[i])break;
	}

	if (i == 4)
	{
		flag = 1;
		printf("It's sended to my IP.\n");
	}

	for (i = 0; i < 4; i++)
	{
		if (ip_hdr->destination_ip[i] != 0xff)break;
	}
	if (i == 4)
	{
		flag = 1;
		printf("It's broadcast IP.\n");
	}

	if (!flag)
		return 0;

	u_int16_t check_sum = calculate_check_sum(ip_hdr, 60);
	if (check_sum == 0xffff || check_sum == 0x0000)
	{
		printf("No error in ip_header.\n");
	}
	else
	{
		printf("Error in ip_header\n");
		return 0;
	}
	return 1;
}

void load_data_to_buffer(u_int8_t *buffer, u_int8_t *ip_data, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		*(buffer + i) = *(ip_data + i);
	}
}

int load_data_to_file(u_int8_t *buffer, int len, FILE *fp)
{
	int res = fwrite(buffer, sizeof(u_int8_t), len, fp);
	if (res != len)
	{
		printf("Write file error!\n");
		return 0;
	}
	fflush(fp);
	return 1;
}

// 计算 ICMP 报文的校验和
u_int16_t calculate_checksum(struct icmp_header *icmp_hdr, int len) {
    u_int32_t sum = 0;
    u_int16_t *ptr = (u_int16_t *)icmp_hdr;

    // 将 ICMP 报文分成 16 位的段进行累加
    for (int i = 0; i < len / 2; i++) {
        sum += *ptr;
        ptr++;
    }

    // 如果 ICMP 报文长度为奇数，处理最后一个字节
    if (len % 2 != 0) {
        sum += *(u_int8_t *)(ptr - 1) << 8;
    }

    // 将 32 位的和折叠成 16 位
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    // 返回校验和的补码
    return (u_int16_t)~sum;
}

void send_icmp_echo_reply(u_int8_t *ip_buffer, int ip_len, struct icmp_header *icmp_request) {
    // 创建 ICMP Echo 应答报文
    struct icmp_header icmp_response;
    icmp_response.type = ICMP_ECHO_REPLY; // ICMP Echo 应答类型
    icmp_response.code = 0;               // 代码总是 0
    icmp_response.id = icmp_request->id;   // 保持标识符不变
    icmp_response.sequence = icmp_request->sequence; // 保持序列号不变

    // 计算校验和
    icmp_response.checksum = 0; // 清零校验和
    u_int16_t checksum = calculate_checksum(&icmp_response, sizeof(struct icmp_header));
    icmp_response.checksum = checksum;

    // 将 ICMP 响应封装到 IP 数据报中
    struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
    ip_hdr->id = htons(ip_id++); // 设置 IP 标识符
    ip_hdr->time_to_live = 64;   // 设置 TTL
    ip_hdr->upper_protocol_type = IPPROTO_ICMP; // 设置上层协议类型为 ICMP

    // 交换源 IP 和目的 IP
    u_int8_t temp_ip[4];
    memcpy(temp_ip, ip_hdr->source_ip, 4);
    memcpy(ip_hdr->source_ip, ip_hdr->destination_ip, 4);
    memcpy(ip_hdr->destination_ip, temp_ip, 4);

    // 发送 IP 数据报
    ethernet_send_packet((u_int8_t *)&icmp_response, ip_buffer + sizeof(struct ip_header), sizeof(struct icmp_header));
}

void icmp_recv(u_int8_t *ip_buffer) {
    struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
    struct icmp_header *icmp_hdr = (struct icmp_header *)(ip_buffer + sizeof(struct ip_header));
	printf("--------------ICMP Protocol-------------------\n");
	printf("ICMP Type: %d\n", icmp_hdr->type);
    printf("ICMP Code: %d\n", icmp_hdr->code);
    printf("ICMP Checksum: %04x\n", icmp_hdr->checksum);
    printf("ICMP ID: %d\n", ntohs(icmp_hdr->id));
    printf("ICMP Sequence: %d\n", ntohs(icmp_hdr->sequence));
    // 检查ICMP类型是否为Echo请求
    if (icmp_hdr->type == ICMP_ECHO_REQUEST) {
        printf("Received ICMP Echo Request\n");

        // 发送ICMP Echo 应答
        send_icmp_echo_reply(ip_buffer, ntohs(ip_hdr->total_length), icmp_hdr);
    }
    printf("-----------------End of ICMP Protocol---------------\n");
}

int network_ipv4_recv(u_int8_t *ip_buffer)
{
	struct ip_header *ip_hdr = (struct ip_header *)ip_buffer;
	int len = ntohs(ip_hdr->total_length) - sizeof(ip_header);


	//check the valid
	if (!is_accept_ip_packet(ip_hdr))
	{
		return 0;
	}

	u_int16_t fragment;
	fragment = ntohs(ip_hdr->fragment_offset);

	int dural = 0;
	if (previous == 0)
	{
		previous = time(NULL);
	}
	else
	{
		//get current time
		current = time(NULL);
		dural = current - previous;
		printf("%d %d\n", current, previous);
		//current time became previous
		previous = current;
	}

	//interval can not larger than 30s
	if (dural >= 30)
	{
		printf("Time Elapsed.\n");
		return 0;
	}

	if ((fragment & 0x2000) && (ip_id == ip_hdr->id))//true means more fragment
	{
		load_data_to_buffer(data_buffer + i, ip_buffer + sizeof(ip_header), len);
		i += len;
		return 1;
	}
	else if (ip_id == ip_hdr->id)
	{
		load_data_to_buffer(data_buffer + i, ip_buffer + sizeof(ip_header), len);
		i += len;
		FILE *fp = fopen("data.txt", "w");
		if (load_data_to_file(data_buffer, i, fp))
		{
			printf("Load to file Succeed.\n");
		}
		fclose(fp);
		//restore the value
		i = 0;
		ip_id++;
	}
	else
	{
		printf("Lost packets.\n");
		//pass the last fragment make move
		i = 0;
		ip_id++;
		return 0;
	}

	printf("--------------IP Protocol-------------------\n");
	printf("IP version: %d\n", (ip_hdr->version_hdrlen & 0xf0));
	printf("Type of service: %02x\n", ip_hdr->type_of_service);
	printf("IP packet length: %d\n", len + sizeof(ip_header));
	printf("IP identification: %d\n", ip_hdr->id);
	printf("IP fragment & offset: %04x\n", ntohs(ip_hdr->fragment_offset));
	printf("IP time to live: %d\n", ip_hdr->time_to_live);
	printf("Upper protocol type: %02x\n", ip_hdr->upper_protocol_type);
	printf("Check sum: %04x\n", ip_hdr->check_sum);
	printf("Source IP: ");
	int i;
	for (i = 0; i < 4; i++)
	{
		if (i)printf(".");
		printf("%d", ip_hdr->source_ip[i]);
	}
	printf("\nDestination IP: ");
	for (i = 0; i < 4; i++)
	{
		if (i)printf(".");
		printf("%d", ip_hdr->destination_ip[i]);
	}
	printf("\n");

	u_int8_t upper_protocol_type = ip_hdr->upper_protocol_type;
	printf("upper_protocol_type:%d\n",upper_protocol_type); 
	switch (upper_protocol_type)
	{
	case IPPROTO_TCP:
		//transport_tcp_recv(buffer);
		break;
	case IPPROTO_UDP:
		//transport_udp_recv(ip_buffer);
		break;
	case IPPROTO_ICMP:
    	icmp_recv(ip_buffer);
    	break;
	}

	printf("-----------------End of IP Protocol---------------\n");
	return 1;
}
