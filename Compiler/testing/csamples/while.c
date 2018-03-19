#include <stdio.h>

int count = 0;
int pow = 1;

int main()
{
    while (count < 5) {
        count = count + 1;
        pow = pow * 2;
        printf(pow);
    }

    return 0;
}