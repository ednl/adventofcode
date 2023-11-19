#include <stdio.h>
#include <stdint.h>

static uint64_t data[128] = {0};
static int index = 0, bitcount;

// Get bit value of len<=64 from bit index in data, update global index
static uint64_t bit(const int len)
{
    const int i = index >> 6, j = index & 63, end = j + len;
    uint64_t val = (data[i] << j) >> (64 - len);
    if (end > 64)
        val |= (data[i + 1] >> (128 - end));
    index += len;
    return val;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-16-input.txt", "r");
    for (uint64_t *a = data; fscanf(f, "%16llX", a++) == 1; );
    int pos = (int)(ftell(f) - 1);  // number of hex characters in input file
    fclose(f);
    int rem = pos & 15;  // remainder out of 16 hex chars (= 64 bit)
    if (rem)
        data[pos >> 4] <<= 64 - (rem << 2);  // correct zeros gap in last value
    bitcount = pos << 2;

    int ver = 0;
    while (index < bitcount) {
        ver += bit(3);
        if (bit(3) == 4) {
            while (bit(1))
                index += 4;
            index += 4;
        } else
            index += bit(1) ? 11 : 15;
    }
    printf("Part 1: %d\n", ver);  // 936

    index = 0;
    uint64_t val = 0;
    printf("Part 2: %llu\n", val);  // ?

    return 0;
}
