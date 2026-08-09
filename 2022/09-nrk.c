/**
 * Advent of Code 2022
 * Day 9: Rope Bridge
 * https://adventofcode.com/2022/day/9
 * By: N-R-K https://codeberg.org/NRK/slashtmp/src/branch/master/AoC/2022/d09-hashtable.c
 * Timer added by E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09-nrk.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09-nrk.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 14.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef TIMER
    #include <string.h>
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-09-input.txt"
#define FSIZE (8192 + 256)  // needed for my input: 8412

#define ABS(X) ((X) > 0 ? (X) : -(X))
#define ARRLEN(x) (sizeof(x) / sizeof(0[x]))

enum { HT_EXP = 24, HT_SIZE = 1ul << HT_EXP };
typedef int32_t Coordinate[2];

static char input[FSIZE];
static Coordinate table[2][HT_SIZE];

static void
update_tpos(int32_t t[static 2], const int32_t h[static 2])
{
    int32_t d[2] = { h[0] - t[0], 1[h] - 1[t] };
    if (ABS(d[0]) > 1 || ABS(d[1]) > 1) {
        t[0] += (d[0] > 0) - (d[0] < 0);
        t[1] += (d[1] > 0) - (d[1] < 0);
    }
}

static void
update_uniq(const int32_t t[static 2], uint64_t *uniq, Coordinate *tbl, size_t tlen)
{
    const uint64_t p = UINT64_C(0x5555133755551773);
    uint64_t hash = (((uint64_t)t[0] * p) + (uint64_t)t[1]) * p;
    uint64_t slot = hash >> (64 - HT_EXP);
    uint64_t step = (hash & (HT_SIZE - 1)) | 0x1;

    if (t[0] == 0 && t[1] == 0) { /* 0,0 implicit uniq */
        return;
    }

    uint64_t cnt = 0;
    for (uint64_t i = slot; 1; i = (i + step) % HT_SIZE) {
        assert(i < tlen);
        if (tbl[i][0] == 0 && tbl[i][1] == 0) {
            /* empty bucket */
            tbl[i][0] = t[0];
            tbl[i][1] = t[1];
            ++*uniq;
            return;
        } else if (tbl[i][0] == t[0] && tbl[i][1] == t[1]) {
            /* not uniq */
            return;
        }
        assert(cnt++ < HT_SIZE); ((void)cnt);
    }
    __builtin_unreachable();
}

static void
flush_buffer(int32_t (*b1)[2], int32_t (*b2)[2], size_t blen, uint64_t uniq[static 2])
{
    for (size_t k = 0; k < blen; ++k)
        update_uniq(b1[k], uniq + 0, table[0], ARRLEN(table[0]));
    for (size_t k = 0; k < blen; ++k)
        update_uniq(b2[k], uniq + 1, table[1], ARRLEN(table[1]));
}

extern int
main(void)
{
    // printf("sizeof(table) = %zuMiB\n", (sizeof table / 1024) / 1024);

    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF or FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(table, 0, sizeof table);  // reset hash table before every run
#endif

    int32_t hpos[2] = {0};
    int32_t tpos[9][2] = {0};
    uint64_t uniq[2] = { 1, 1 }; /* 0,0 is automatically uniq */
    int32_t buf1[64][2], buf2[ARRLEN(buf1)][2];
    int buf_head = 0;

    // for (char buf[32]; fgets(buf, sizeof buf, stdin); /* no-op */) {
    for (const char *c = input; *c; ) {
        int32_t mov[2];
        switch (*c) {
        case 'D': mov[0] =  0; mov[1] =  1; break;
        case 'L': mov[0] = -1; mov[1] =  0; break;
        case 'R': mov[0] =  1; mov[1] =  0; break;
        case 'U': mov[0] =  0; mov[1] = -1; break;
        default: __builtin_unreachable(); break;
        }
        int32_t cnt = *(c + 2) & 15;
        if (*(c + 3) == '\n')
            c += 4;
        else {
            cnt = cnt * 10 + (*(c + 3) & 15);
            c += 5;
        }
        // for (char *p = buf + 2; *p != '\n'; ++p) {
        //     cnt = (cnt * 10) + (*p - '0');
        // }

        for (int32_t i = 0; i < cnt; ++i) {
            const int32_t (*p)[2] = &hpos;
            hpos[0] += mov[0]; hpos[1] += mov[1];
            for (size_t k = 0; k < ARRLEN(tpos); ++k) {
                update_tpos(tpos[k], *p);
                p = tpos + k;
            }
            if (buf_head == ARRLEN(buf1)) {
                flush_buffer(buf1, buf2, ARRLEN(buf1), uniq);
                buf_head = 0;
            }
            buf1[buf_head][0] = tpos[0][0]; buf1[buf_head][1] = tpos[0][1];
            buf2[buf_head][0] = tpos[8][0]; buf2[buf_head][1] = tpos[8][1];
            ++buf_head;
        }
    }
    flush_buffer(buf1, buf2, buf_head, uniq);

    // Part 1: ex1=13, ex2=88, input=6745
    // Part 2: ex1= 1, ex2=36, input=2793
    printf("%"PRIu64" %"PRIu64"\n", uniq[0], uniq[1]);

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
