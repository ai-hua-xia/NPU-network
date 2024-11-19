#include "ARP_Cache_Table.h"
#include "Resource.h"

// 外部变量声明
extern u_int8_t local_mac[6]; // 本地MAC地址
extern u_int8_t local_ip[4];  // 本地IP地址
extern u_int8_t gateway_ip[4]; // 网关IP地址
extern u_int8_t netmask[4];   // 子网掩码
extern u_int8_t dns_server_ip[4]; // DNS服务器IP地址
extern u_int8_t dhcp_server_ip[4]; // DHCP服务器IP地址

// ARP表头
arp_table_header arp_table; // 定义一个全局的ARP表头实例

// 创建一个ARP节点
struct arp_node* make_arp_node(u_int8_t *ip, u_int8_t *mac, int state)
{
    int i;
    struct arp_node *node = (struct arp_node *)malloc(sizeof(struct arp_node)); // 分配内存给新节点
    for (i = 0; i < 4; i++)
    {
        node->ip[i] = ip[i]; // 复制IP地址
    }

    for (i = 0; i < 6; i++)
    {
        node->mac[i] = mac[i]; // 复制MAC地址
    }
    node->state = state; // 设置节点状态
    node->next = NULL; // 初始化下一个节点指针为NULL
    return node; // 返回新创建的节点
}

// 初始化ARP表
void init_arp_table()
{
    struct arp_node *node;
    node = make_arp_node(local_ip, local_mac, STATIC_STATE); // 创建包含本地IP和MAC地址的静态状态节点

    arp_table.queue = node; // 设置队列头节点
    arp_table.head = node; // 设置表头节点
    arp_table.tail = node; // 设置表尾节点
}

// 插入一个ARP节点
void insert_arp_node(struct arp_node *node)
{
    if (!is_existed_ip(node->ip)) // 如果IP地址不存在于ARP表中
    {
        arp_table.tail->next = node; // 将新节点添加到表尾
        arp_table.tail = node; // 更新表尾节点
    }
}

// 删除一个ARP节点
int delete_arp_node(struct arp_node *node)
{
    struct arp_node *pre = arp_table.head; // 前一个节点指针初始化为头节点
    struct arp_node *p = pre->next; // 当前节点指针初始化为头节点的下一个节点
    int flag = 1; // 用于标记是否找到要删除的节点
    while (p != NULL)
    {
        int i;
        flag = 1; // 重置标志位
        for (i = 0; i < 4; i++) // 比较IP地址
        {
            if (node->ip[i] != p->ip[i])
            {
                flag = 0; // 如果不匹配，设置标志位为0
                break;
            }
        }

        for (i = 0; i < 6; i++) // 比较MAC地址
        {
            if (node->mac[i] != p->mac[i])
            {
                flag = 0; // 如果不匹配，设置标志位为0
                break;
            }
        }

        if (flag) // 如果找到了匹配的节点
        {
            pre->next = p->next; // 跳过当前节点
            free(p); // 释放节点内存
            break; // 结束循环
        }

        pre = p; // 移动前一个节点指针
        p = p->next; // 移动当前节点指针
    }
    if (flag)
    {
        printf("delete arp node succeed!!!\n"); // 打印删除成功消息
        return 1; // 返回成功
    }
    else
    {
        printf("Failed delete\n"); // 打印删除失败消息
        return 0; // 返回失败
    }
}

// 检查IP是否存在于ARP表中
u_int8_t* is_existed_ip(u_int8_t *destination_ip)
{
    struct arp_node *p = arp_table.head; // 当前节点指针初始化为头节点
    int flag = 1; // 用于标记是否找到匹配的IP地址
    while (p != NULL)
    {
        int i;
        flag = 1; // 重置标志位
        for (i = 0; i < 4; i++) // 比较IP地址
        {
            if (p->ip[i] != destination_ip[i])
            {
                flag = 0; // 如果不匹配，设置标志位为0
                break;
            }
        }

        if (flag) // 如果找到了匹配的IP地址
        {
            return p->mac; // 返回对应的MAC地址
        }
        p = p->next; // 移动当前节点指针
    }
    return NULL; // 如果没有找到匹配的IP地址，返回NULL
}

// 更新ARP节点
int update_arp_node(struct arp_node *node)
{
    u_int8_t *mac = is_existed_ip(node->ip); // 查找IP地址是否已存在
    if (mac) // 如果找到了
    {
        int i;
        for (i = 0; i < 6; i++) // 更新MAC地址
        {
            mac[i] = node->mac[i];
        }
        printf("Update succeed.\n"); // 打印更新成功消息
        return 1; // 返回成功
    }
    else
    {
        printf("Update failed.\n"); // 打印更新失败消息
        return 0; // 返回失败
    }
}

// 输出ARP表
void output_arp_table()
{
    struct arp_node *p = arp_table.head; // 当前节点指针初始化为头节点
    while (p != NULL)
    {
        int i;
        for (i = 0; i < 4; i++) // 输出IP地址
        {
            if (i) printf(".");
            printf("%d", p->ip[i]);
        }
        printf("\t");
        for (i = 0; i < 6; i++) // 输出MAC地址
        {
            if (i) printf("-");
            printf("%02x", p->mac[i]);
        }
        printf("\n");

        p = p->next; // 移动当前节点指针
    }
}