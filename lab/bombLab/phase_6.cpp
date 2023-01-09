#include <cstdio>
#include <cstdlib>
#include <map>
#define debug(node) printf("para1: %x ,para2: %x ,next: %llx\n", node.para1, node.para2, node.next)
typedef long long address;
struct Node
{
    int para1;
    int para2;
    address next;
};
std::map<address, Node> nodes;
void bomb()
{
    printf("bomb!!\n");
    exit(0);
}
int main()
{
    address root = 0x6032d0;
    nodes[0x6032d0] = {0x14c, 0x1, 0x6032e0};
    nodes[0x6032e0] = {0xa8, 0x2, 0x6032f0};
    nodes[0x6032f0] = {0x39c, 0x3, 0x603300};
    nodes[0x603300] = {0x2b3, 0x4, 0x603310};
    nodes[0x603310] = {0x1dd, 0x5, 0x603320};
    nodes[0x603320] = {0x1bb, 0x6, 0x0};

    int nums[] = {4, 3, 2, 1, 6, 5};

    // 第一个大循环套小循环
    for (int i = 0; i < 6; i++)
    {
        if (nums[i] > 6)
            bomb();
        for (int j = i + 1; j < 6; j++)
            if (nums[i] == nums[j])
                bomb();
    }

    // 第二个循环
    for (int i = 0; i < 6; i++)
        nums[i] = 7 - nums[i];

    //第三个循环，将node入栈

    address addresses[6];
    int idx = 0;
    for (int i = 0; i < 6; i++)
    {
        int eax = 1;
        int ecx = nums[i];
        address edx = root;
        while (eax < ecx)
        {
            edx = nodes[edx].next;
            eax++;
        }
        addresses[idx++] = edx;
    }

    // 第四个循环，从新连接链表
    address newRoot = addresses[0];
    for (int rax = 1; rax < idx; rax++)
    {
        address rdx = addresses[rax];
        Node &rcx = nodes[newRoot];
        rcx.next = rdx;
        newRoot = rdx;
    }
    nodes[newRoot].next = 0;

    // 检查
    address debugRoot = addresses[0];
    debug(nodes[root]);
    for (int i = 1; i < 6; i++)
    {
        debugRoot = nodes[debugRoot].next;
        debug(nodes[debugRoot]);
    }

    // 第五个模拟，检查是否降序
    address rbx = addresses[0];
    for (int ebp = 5; ebp >= 0; ebp--)
    {
        Node rax = nodes[nodes[rbx].next];
        if (rax.para1 >= nodes[rbx].para1)
            bomb();
        else
        {
            rbx = nodes[rbx].next;
        }
    }

    // 完成
    printf("well done!\n");

    return 0;
}