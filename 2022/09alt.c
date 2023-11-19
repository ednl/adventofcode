#include <stdio.h>
#include <stdlib.h>  // abs
#include <string.h>  // memmove, memset
#include <stdint.h>  // int16_t, uint32_t
#include <stdbool.h>

#define F "../aocinput/2022-09-input.txt"
#define K 10    // max number of knots in rope
#define N 2000  // max number of lines in input file
#define M 8192  // max number of different locations visited by tail

typedef struct Vec {
    int16_t x, y;
} Vec;

typedef struct Motion {
    Vec dir;
    int16_t len;
} Motion;

typedef union U32 {
    Vec vec;
    uint32_t u32;
} U32;

static Motion motion[N];
static Vec rope[K];
static uint32_t seen[M];
static size_t seenlen;

static uint32_t hash(const Vec v)
{
    return (U32){v}.u32;
}

// Insert new hash value and return true.
// Return false if value already present or can't be inserted.
static bool binsert(const uint32_t k, uint32_t * const a, size_t * const curlen, const size_t maxlen)
{
    if (!a || !curlen || *curlen >= maxlen) {
        putchar('s');  // major error
        return false;
    }
    if (!*curlen || k > a[*curlen - 1]) {
        a[(*curlen)++] = k;
        return true;
    }
    if (k < a[0]) {
        memmove(a + 1, a, (*curlen)++ * sizeof *a);
        a[0] = k;
        return true;
    }
    size_t l = 0, r = *curlen - 1;
    if (k == a[l] || k == a[r])
        return false;
    // Now always true: a[l] < k < a[r]
    while (r - l != 1) {
        size_t m = ((l + r) >> 1);
        if      (k > a[m]) l = m;
        else if (k < a[m]) r = m;
        else               return false;
    }
    memmove(a + r + 1, a + r, ((*curlen)++ - r) * sizeof *a);
    a[r] = k;
    return true;
}

static size_t simul(const size_t motioncount, const int knots)
{
    seenlen = 0;
    memset(rope, 0, sizeof rope);  // all knots start at the origin
    binsert(0, seen, &seenlen, M);
    for (size_t i = 0; i < motioncount; ++i) {
        for (int16_t j = 0; j < motion[i].len; ++j) {
            rope[0].x += motion[i].dir.x;
            rope[0].y += motion[i].dir.y;
            for (int k = 1; k < knots; ++k) {
                Vec dif = (Vec){rope[k - 1].x - rope[k].x, rope[k - 1].y - rope[k].y};
                if (abs(dif.x) == 2 || abs(dif.y) == 2) {
                    rope[k].x += dif.x >= 1 ? 1 : (dif.x <= -1 ? -1 : 0);
                    rope[k].y += dif.y >= 1 ? 1 : (dif.y <= -1 ? -1 : 0);
                    if (k == knots - 1)
                        binsert(hash(rope[k]), seen, &seenlen, M);
                } else
                    break;
            }
        }
    }
    return seenlen;
}

static size_t read(const char * const fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char c;
    int16_t len;
    size_t n = 0;
    while (n < N && fscanf(f, "%c %hd ", &c, &len) == 2) {
        Vec dir = {0};
        switch (c) {
            case 'D': dir.y = -1; break;
            case 'L': dir.x = -1; break;
            case 'R': dir.x =  1; break;
            case 'U': dir.y =  1; break;
        }
        motion[n++] = (Motion){dir, len};
    }
    fclose(f);
    return n;
}

int main(void)
{
    size_t motioncount = read(F);
    printf("Part 1: %zu\n", simul(motioncount,  2));  // 6745
    printf("Part 2: %zu\n", simul(motioncount, 10));  // 2793
    return 0;
}
