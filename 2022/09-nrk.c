#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "../startstoptimer.h"

#define ABS(X) ((X) > 0 ? (X) : -(X))
#define ARRLEN(x) (sizeof(x) / sizeof(0[x]))

enum { HT_EXP = 24, HT_SIZE = 1ul << HT_EXP };
typedef int32_t Coordinate[2];
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
    starttimer();

    int32_t hpos[2] = {0};
    int32_t tpos[9][2] = {0};
    uint64_t uniq[2] = { 1, 1 }; /* 0,0 is automatically uniq */
    int32_t buf1[64][2], buf2[ARRLEN(buf1)][2];
    int buf_head = 0;

    // printf("sizeof(table) = %zuMiB\n", (sizeof table / 1024) / 1024);
    for (char buf[32]; fgets(buf, sizeof buf, stdin); /* no-op */) {
        int32_t mov[2], cnt = 0;
        switch (buf[0]) {
        case 'R': mov[0] =  1; mov[1] =  0; break;
        case 'L': mov[0] = -1; mov[1] =  0; break;
        case 'U': mov[0] =  0; mov[1] = -1; break;
        case 'D': mov[0] =  0; mov[1] =  1; break;
        default: __builtin_unreachable(); break;
        }
        for (char *p = buf + 2; *p != '\n'; ++p) {
            cnt = (cnt * 10) + (*p - '0');
        }

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
    printf("part1: %"PRIu64"\npart2: %"PRIu64"\n", uniq[0], uniq[1]);
    printf("Time: %.0f µs\n", stoptimer_us());  // M1: 4657 µs
    return 0;
}
