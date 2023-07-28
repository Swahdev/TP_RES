#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    uint32_t x = 0x12345678;
    uint32_t y = htonl(x);
    printf("x = %x", x);
    printf("y = %x", y);
    /*
    if(*(char *)&y == 0x12)
        printf("Big Endian\n");
    else
        printf("Little Endian\n");
    return 0;
    */
   // htonl convertit en little endian
}
