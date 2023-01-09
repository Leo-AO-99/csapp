#include <stdio.h>
#include <stdlib.h>
void func5(char rbx[])
{
    char temp[] = "maduiersnfotvbyl";
    char rsp[20];
    for (int rax = 0; rax < 6; rax++)
    {
        int rdx = rbx[rax];
        rdx &= 0xf;
        rdx = temp[rdx];
        rsp[rax] = rdx;
    }
    rsp[6] = 0;
    printf("%s\n", rsp);
}
int main()
{
    char rbx[20];
    rbx[0] = 41;
    rbx[1] = 63;
    rbx[2] = 62;
    rbx[3] = 37;
    rbx[4] = 38;
    rbx[5] = 39;
    rbx[6] = 0;
    printf("%s\n", rbx);
    func5(rbx);
    return 0;
}