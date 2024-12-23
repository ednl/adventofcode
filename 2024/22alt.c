#include <stdio.h>
#include <stdlib.h>  // malloc, realloc, free
#include <string.h>  // memmove
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-22-example1.txt"
    #define N 4
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-22-example2.txt"
    #define N 4
#elif EXAMPLE == 3
    #define FNAME "22.txt"
    #define N 1
#else
    #define FNAME "../aocinput/2024-22-input.txt"
    #define N 2268
#endif

#define LEN 2000  // series of consecutive secret numbers generated
#define M24 ((UINT32_C(1) << 24) - 1)  // modulo 16777216 = 24 bit
#define TSIZE 2048

typedef struct entry {
    uint32_t key, val;
} Entry;

typedef struct table {
    Entry *dat;
    size_t siz, len;
} Table;

static Table inittable(const size_t size)
{
    return (Table){.dat = malloc(size * sizeof (Entry)), .siz = size, .len = 0};
}

static bool freetable(Table *const t)
{
    if (!t)
        return false;
    free(t->dat);
    *t = (Table){0};
    return true;
}

static bool growtable(Table *const t)
{
    if (!t)
        return false;
    const size_t newsiz = t->siz ? t->siz << 1 : TSIZE;
    Entry *tmp = realloc(t->dat, newsiz * sizeof *t->dat);
    if (!tmp)
        return false;
    t->dat = tmp;
    t->siz = newsiz;
    return true;
}

// Insert into seq if not exist
static bool binsert(Table *const t, const Entry e, const bool add)
{
    if (t->len >= t->siz && !growtable(t))
        return false;
    if (!t->len || e.key > t->dat[t->len - 1].key) {
        t->dat[t->len++] = e;
        return true;
    }
    if (e.key < t->dat[0].key) {
        memmove(t->dat + 1, t->dat, t->len++ * sizeof *t->dat);
        t->dat[0] = e;
        return true;
    }
    size_t l = 0, r = t->len - 1;
    if (e.key == t->dat[l].key) {
        if (add)
            t->dat[l].val += e.val;
        return add;
    }
    if (e.key == t->dat[r].key) {
        if (add)
            t->dat[r].val += e.val;
        return add;
    }
    // Now always true: seq[l].key < key < seq[r].key
    while (r - l != 1) {
        const int m = ((l + r) >> 1);
        if      (e.key > t->dat[m].key) l = m;
        else if (e.key < t->dat[m].key) r = m;
        else {
            if (add)
                t->dat[m].val += e.val;
            return add;
        }
    }
    memmove(t->dat + r + 1, t->dat + r, (t->len++ - r) * sizeof *t->dat);
    t->dat[r] = e;
    return true;
}

static uint32_t xorshift(uint32_t x)
{
    x ^= x << 6;     // mul by 64, mix
    x &= M24;        // prune
    x ^= x >> 5;     // div by 32, mix
    x ^= x << 11;    // (prune not needed) mul by 2048, mix
    return x & M24;  // prune, return
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }

#ifdef TIMER
    starttimer();
#endif

    Table seq = inittable(TSIZE);
    Table sum = inittable(TSIZE * 32);
    uint64_t buysum = 0;
    uint32_t key = 0, x;
    while (fscanf(f, "%u ", &x) == 1) {
        seq.len = 0;
        uint32_t prev = x % 10;
        for (int i = 0; i < 3; ++i) {
            x = xorshift(x);
            const uint32_t val = x % 10;  // value 0..9
            key = key << 6 | (9 + val - prev);  // index 0..18 fits in 5 bit, use 6
            prev = val;
        }
        for (int i = 3; i < LEN; ++i) {
            x = xorshift(x);
            const uint32_t val = x % 10;  // value 0..9
            key = (key << 6 | (9 + val - prev)) & M24;  // index 0..18 fits in 5 bit, use 6, keep at 4x6=24 bit
            binsert(&seq, (Entry){key, val}, false);
            prev = val;
        }
        buysum += x;  // part 1
        for (size_t i = 0; i < seq.len; ++i)
            binsert(&sum, seq.dat[i], true);
    }
    fclose(f);
    printf("%"PRIu64"\n", buysum);  // example1: 37327623, example2: 37990510, input: 19150344884
    uint32_t max = 0;
    for (size_t i = 0; i < sum.len; ++i)
        if (sum.dat[i].val > max)
            max = sum.dat[i].val;
    printf("%u\n", max);  // example1: 24, example2: 23, input: 2121
    freetable(&seq);
    freetable(&sum);

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
