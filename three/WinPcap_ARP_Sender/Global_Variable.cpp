#include "Header_Include.h"

// 本地MAC地址
u_int8_t local_mac[6] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };

// 本地IP地址
u_int8_t local_ip[4] = { 10, 10, 10, 4 };

// 网关IP地址
u_int8_t gateway_ip[4] = { 10, 10, 11, 1 };

// 子网掩码
u_int8_t netmask[4] = { 255, 255, 248, 0 };

// DNS服务器IP地址
u_int8_t dns_server_ip[4] = { 211, 137, 130, 3 };

// DHCP服务器IP地址
u_int8_t dhcp_server_ip[4] = { 111, 20, 62, 57 };

// 广播MAC地址
u_int8_t broadcast_mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

// pcap句柄，用于数据包捕获
pcap_t *handle;

// 以太网帧的上层协议长度
int ethernet_upper_len;

// 目标IP地址
u_int8_t target_ip[4] = { 10, 10, 10, 10 };