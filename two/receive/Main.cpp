#include "Network_ipv4_recv.h"

int main()
{
  pcap_t *handle;
  pcap_if_t *alldevs, *device;
  char *device_name;
  char error_buffer[PCAP_ERRBUF_SIZE];

  // 查找所有可用的网络设备
  if (pcap_findalldevs(&alldevs, error_buffer) == -1) {
    printf("Error finding devices: %s\n", error_buffer);
    return -1;
  }

  // 显示可用设备列表
  printf("receive_Available adapters:\n");
  int id = 1;
  for (device = alldevs; device != NULL; device = device->next) {
    printf("\n%d. %s\n", id++, device->name);  // 显示设备名称
    printf("--- %s\n", device->description);  // 显示设备描述
  }
  printf("\n");

  // 用户选择网卡
  int choice;
  printf("Enter the adapter id (1-%d): ", id - 1);
  scanf("%d", &choice);

  // 验证用户输入的编号是否有效
  if (choice < 1 || choice >= id) {
    printf("Invalid choice.\n");
    pcap_freealldevs(alldevs);
    return -1;
  }

  // 根据用户选择的编号找到对应的设备
  device = alldevs;
  for (int i = 1; i < choice; i++) {
    device = device->next;  // 迭代直到找到用户选择的设备
  }

  // 打开选定的设备进行数据包捕获
  handle = pcap_open_live(device->name, 65536, 1, 1000, error_buffer);
  if (handle == NULL) {
    printf("Open adapter failed: %s\n", error_buffer);
    pcap_freealldevs(alldevs);
    return -1;
  }

  // 捕获数据包，回调处理函数为 ethernet_protocol_packet_callback
  pcap_loop(handle, 0, ethernet_protocol_packet_callback, NULL);

  pcap_close(handle);
  return 0;
}