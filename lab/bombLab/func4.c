#include <stdio.h>
#include <stdlib.h>
int func4(int edi, int esi, int edx)
{
    int eax = edx;
    eax -= esi;
    int ecx = eax;
    ecx >>= 31;
    eax += ecx;
    eax >>= 1;
    ecx = eax + 1 * esi;
    if (edi < ecx)
    {
        edx = ecx - 1;
        eax = func4(edi, esi, edx);
        eax += eax;
        return eax;
    }
    else
    {
        // ecx <= edi
        eax = 0;
        if (edi > ecx)
        {
            esi = ecx + 1;
            eax = func4(edi, esi, edx);
            eax = eax + 1 * eax + 1;
            return eax;
        }
        else
        {
            return eax;
        }
    }
}
int main()
{
    for (int n = 0; n <= 14; n++)
    {
        int ret = func4(n, 0, 14);
        if (ret == 0)
            printf("%d ", n);
    }
    return 0;
}