#include <stdio.h>
#include <stdint.h>
#include <string.h>  // memcpy
#include <inttypes.h>

#define FNAME "../aocinput/2021-14-input.txt"
#define FSIZE 1024  // needed for my input: 822
#define RULES 100
#define ALPH (('Z' & 31) + 1)
#define INISIZE 20

static char input[FSIZE];

static uint8_t node[RULES][2];
static uint8_t insert[RULES];
static int rule[ALPH][ALPH];
static int left[RULES];
static int right[RULES];
static uint64_t count[2][RULES];

// Swap pointers (not values)
static void swap(uint64_t *restrict *p, uint64_t *restrict *q)
{
    uint64_t *const tmp = *p;
    *p = *q;
    *q = tmp;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

    const char *c = input + INISIZE + 2;
    for (int i = 0; i < RULES; c += 8, ++i) {
        uint64_t data;
        memcpy(&data, c, sizeof data);
        data &= UINT64_C(0x001f000000001f1f);  // assumes little-endian
        memcpy(node[i], &data, 2);
        rule[data & 255][data >> 8 & 255] = i;
        insert[i] = data >> 48;
    }

    for (int i = 0 ; i < RULES; ++i) {
         left[i] = rule[node[i][0]][ insert[i]];
        right[i] = rule[ insert[i]][node[i][1]];
    }
    for (c = input; c < input + INISIZE - 1; ++c)
        count[0][rule[*c & 31][*(c + 1) & 31]]++;

    uint64_t *curr = count[0], *next = count[1];
    for (int steps = 0; steps < 10; ++steps) {
        memset(next, 0, sizeof count[0]);
        for (int i = 0; i < RULES; ++i) {
            next[ left[i]] += curr[i];
            next[right[i]] += curr[i];
        }
        swap(&curr, &next);
    }

    uint64_t hist[ALPH] = {0};
    for (int i = 0; i < RULES; ++i) {
        hist[node[i][0]] += curr[i];
        hist[node[i][1]] += curr[i];
    }
    uint64_t min = INT64_MAX;
    uint64_t max = 0;
    for (int i = 1; i < ALPH; ++i) {
        if (hist[i] < min) min = hist[i];
        if (hist[i] > max) max = hist[i];
    }
    printf("%"PRIu64"\n", (max - min) >> 1);  // 2194
}
