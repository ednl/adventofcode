/**
 * Advent of Code 2023
 * Day 12:
 * https://adventofcode.com/2023/day/12
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdbool.h>   // bool

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-12-example.txt"
#define N 6  // different spring rows (=lines in input file)
#define M 4  // max number of spring clusters (=field count in CSV)
#else
#define NAME "../aocinput/2023-12-input.txt"
#define N 1000
#define M 6
#endif

typedef struct springs {
    int len, operational, damaged, unknown, first, last;
    int group[M];
    int groups;
} Springs;

static Springs springs[N];

static void putbin(const int n, int len)
{
    if (len > 0) {
        while (len--)
            fputc('0' | (n >> len & 1), stdout);
        fputc('\n', stdout);
    }
}

static void render(char* buf, const Springs* const row)
{
    for (int mask = 1 << (row->len - 1); mask; mask >>= 1, ++buf) {
        if  (row->operational & mask) *buf = '.';
        else if (row->damaged & mask) *buf = '#';
        else if (row->unknown & mask) *buf = '?';
        else *buf = '_';
    }
    *buf = '\0';
}

static int first(const Springs* const row)
{
    const int must = row->damaged;
    const int deny = row->operational;
    int pat = 0, bit = 1 << (row->len - 1);
    for (int i = 0; i < row->groups; ++i) {  // all clusters forward
        while (bit & deny)
            bit >>= 1;
        if (!bit)
            return 0;  // can't start cluster
        int cluster = 0, count = row->group[i];
        while (bit && count) {
            cluster |= bit;
            bit >>= 1;  // next bit to set
            --count;
        }
        if (count)
            return 0;  // ran out of bits to set
        int lsb = bit ? bit << 1 : 1;  // last bit of cluster
        while ((cluster & deny || bit & must) && lsb) {
            cluster >>= 1;
            lsb >>= 1;
            bit >>= 1;
        }
        if (cluster & deny || bit & must || !lsb)
            return 0;  // cluster doesn't fit
        pat |= cluster;
        bit >>= 1;  // skip 1 to start of next cluster
    }
    return pat;
}

static int last(const Springs* row)
{
    const int must = row->damaged;
    const int deny = row->operational;
    const int allow = (1 << row->len) - 1;
    int pat = 0, bit = 1;
    for (int i = row->groups - 1; i >= 0; --i) {  // all clusters backward
        while (bit & deny)
            bit <<= 1;
        if (!(bit & allow))
            return 0;  // can't start cluster
        int cluster = 0, count = row->group[i];
        while ((bit & allow) && count) {
            cluster |= bit;
            bit <<= 1;  // next bit to set
            --count;
        }
        if (count)
            return 0;  // ran out of bits to set
        int msb = bit >> 1;  // first bit of cluster
        while ((cluster & deny || bit & must) && msb & allow) {
            cluster <<= 1;
            msb <<= 1;
            bit <<= 1;
        }
        if (cluster & deny || bit & must || !(msb & allow))
            return 0;  // cluster doesn't fit
        pat |= cluster;
        bit <<= 1;  // skip 1 to start of next cluster
    }
    return pat;
}

static void interpret(Springs* const row, const char* s)
{
    *row = (Springs){0};
    while (*s && *s != ' ') {
        row->len++;
        row->operational <<= 1;
        row->damaged <<= 1;
        row->unknown <<= 1;
        switch (*s++) {
            case '.': row->operational |= 1; break;
            case '#': row->damaged |= 1; break;
            case '?': row->unknown |= 1; break;
        }
    }
    if (*s == ' ')
        ++s;  // skip ' '
    while (*s && *s != '\n') {
        int val = 0;
        while (*s >= '0' && *s <= '9')
            val = val * 10 + (*s++ & 15);
        row->group[row->groups++] = val;
        if (*s == ',')
            ++s;  // skip ','
    }
    row->first = first(row);
    row->last = last(row);
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int n = 0;
    char buf[64];
    while (n < N && fgets(buf, sizeof buf, f))
        interpret(&springs[n++], buf);
    fclose(f);

    int todo = 0;
    for (int i = 0; i < n; ++i) {
        const int dif = springs[i].first ^ springs[i].last;
        if (dif) {
            ++todo;
            render(buf, &springs[i]);
            printf("%s", buf);
            for (int j = 0; j < springs[i].groups; ++j)
                printf(" %d", springs[i].group[j]);
            printf("\n");
            putbin(springs[i].first, springs[i].len);
            putbin(springs[i].last , springs[i].len);
            putbin(dif, springs[i].len);
            printf("\n");
        }
    }
    printf("todo=%d\n", todo);
    return 0;
}
