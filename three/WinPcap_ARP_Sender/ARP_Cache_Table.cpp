#include "ARP_Cache_Table.h" // 包含ARP缓存表相关的头文件
#include "Resource.h" // 包含资源相关的头文件

// 外部变量声明
// 这些变量通常会在其他地方定义，这里只是声明它们是外部的，可以在这个文件中使用。
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
        node->ip[i] = ip[i]; // 复制IP地址到节点
    }

    for (i = 0; i < 6; i++)
    {
        node->mac[i] = mac[i]; // 复制MAC地址到节点
    }
    node->state = state; // 设置节点状态
    node->next = NULL; // 新节点的下一个指针设为NULL
    return node; // 返回新创建的节点
}

// 初始化ARP表
void init_arp_table()
{
    struct arp_node *node; // 声明一个新的节点指针
    node = make_arp_node(local_ip, local_mac, STATIC_STATE); // 创建一个节点，包含本地IP和MAC地址

    arp_table.queue = node; // 将队列设置为这个新节点
    arp_table.head = node;  // 表头指向这个新节点
    arp_table.tail = node;  // 表尾也指向这个新节点
}

// 插入新的ARP节点到表尾
void insert_arp_node(struct arp_node *node)
{
    if (!is_existed_ip(node->ip)) // 如果IP地址不存在于表中
    {
        arp_table.tail->next = node; // 将表尾的下一个节点设为新节点
        arp_table.tail = node;       // 更新表尾为新节点
    }
}

// 删除ARP节点（头结点无法删除）
int delete_arp_node(struct arp_node *node)
{
    struct arp_node *pre = arp_table.head; // 第一个节点
    struct arp_node *p = pre->next;       // 当前节点
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

    if (flag) // 如果成功删除
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

// 检查IP地址是否存在于ARP表中
u_int8_t* is_existed_ip(u_int8_t *destination_ip)
{
    struct arp_node *p = arp_table.head; // 从表头开始
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

        p = p->next; // 移动到下一个节点
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
    struct arp_node *p = arp_table.head; // 从表头开始
    while (p != NULL)
    {
        int i;
        for (i = 0; i < 4; i++) // 打印IP地址
        {
            if (i) printf(".");
            printf("%d", p->ip[i]);
        }
        printf("\t"); // 打印分隔符
        for (i = 0; i < 6; i++) // 打印MAC地址
        {
            if (i) printf("-");
            printf("%02x", p->mac[i]);
        }
        printf("\n"); // 打印换行符

        p = p->next; // 移动到下一个节点
    }
}
