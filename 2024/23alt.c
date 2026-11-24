#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FNAME "../aocinput/2024-23-input.txt"
#define N 3380  // lines in input file
#define FSIZE (N * 6)  // 5 characters per line +newline
#define RANGE (26 * 26)
#define SEENSZ ((RANGE + 63) >> 6)  // how many u64 in bitset (= 11)
#define EDGESZ (((RANGE * RANGE) + 63) >> 6)  // how many u64 in bitset (= 7141)

static char input[FSIZE];
static uint64_t seen[SEENSZ];
static uint64_t edge[EDGESZ];
static uint16_t nb[RANGE][14];

static unsigned nodeid(const char *const s)
{
    return *s * 26 + *(s + 1) - 27 * 'a';
}

static void writenode(char *const s, const unsigned id)
{
    *s       = id / 26 + 'a';
    *(s + 1) = id % 26 + 'a';
    *(s + 2) = ',';
}

static void markseen(const unsigned id)
{
    seen[id >> 6] |= UINT64_C(1) << (id & 63);
}

static bool isseen(const unsigned id)
{
    return seen[id >> 6] >> (id & 63) & 1;
}

static void markedge(const unsigned a, const unsigned b)
{
    const unsigned ix = a * RANGE + b;
    edge[ix >> 6] |= UINT64_C(1) << (ix & 63);
}

static bool isedge(const unsigned a, const unsigned b)
{
    const unsigned ix = a * RANGE + b;
    return edge[ix >> 6] >> (ix & 63) & 1;
}

int main(void)
{
    {
        FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(input, sizeof input, 1, f);  // read whole file in one go
        fclose(f);
    }

    {
        const char *s = input;
        for (int i = 0; i < N; s += 6, ++i) {
            const unsigned a = nodeid(s);
            const unsigned b = nodeid(s + 3);
            markseen(a);
            markseen(b);
            markedge(a, b);
            markedge(b, a);
        }
    }

    for (unsigned i = ('t' - 'a') * 26; i < ('u' - 'a') * 26; ++i)
        for (unsigned j = 0; j < RANGE; ++j)
            if (isedge(i, j))
                ;
}
