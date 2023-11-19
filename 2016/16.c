#include <stdio.h>
#include <stdlib.h>  // malloc, free
#include <string.h>  // strlen
#include <stdint.h>  // uint8_t

#define EXAMPLE 0
#if EXAMPLE == 1
#define INPUT "10000"
#define DISK1 20
#define DISK2 20
#else
#define INPUT "11011110011011101"
#define DISK1 272
#define DISK2 35651584
#endif

static void show(uint8_t *disk, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        putchar("01"[disk[i]]);
    putchar('\n');
}

static void checksum(const char * const initstate, const size_t disksize)
{
    size_t len = strlen(initstate);
    size_t size = len;
    while (size < disksize)
        size = size * 2 + 1;
    uint8_t *disk = malloc(size);
    if (!disk)
        return;
    for (size_t i = 0; i < len; ++i)
        disk[i] = (uint8_t)(initstate[i] - '0');

    while (len < disksize) {
        size_t i = 0, j = len << 1;
        while (i < j)
            disk[j--] = !disk[i++];
        disk[len] = 0;
        len = (len << 1) + 1;
    }

    len = disksize;
    while (!(len & 1)) {
        len >>= 1;
        for (size_t i = 0, j = 0; i < len; j = ++i << 1)
            disk[i] = disk[j] == disk[j + 1];
    }
    show(disk, len);
    free(disk);
}

int main(void)
{
    printf("Part 1: "); checksum(INPUT, DISK1);  // example=01100, input=00000100100001100
    printf("Part 2: "); checksum(INPUT, DISK2);  // example=    -, input=00011010100010010
    return 0;
}
