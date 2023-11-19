#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>  // int64_t
#include <stdlib.h>  // qsort, llabs

#define N 1000

typedef struct _pos_t {
    int64_t x, y, z;
} pos_t;

typedef struct _bot_t {
    pos_t p;
    int64_t r;
} bot_t;

typedef struct _botcounter_t {
    int64_t dist;
    int count;
} botcounter_t;

static bot_t bot[N];
static botcounter_t line[N * 2];

static size_t read(void)
{
    FILE *f = fopen("../aocinput/2018-23-input.txt", "r");
    if (!f)
        return 0;
    size_t n = 0;
    int64_t x, y, z, r;
    while (n < N && !feof(f) && fscanf(f, "pos=<%lld,%lld,%lld>, r=%lld ", &x, &y, &z, &r) == 4)
        bot[n++] = (bot_t){(pos_t){x, y, z}, r};
    fclose(f);
    return n;
}

static int range_desc(const void * a, const void * b)
{
    const bot_t *p = a, *q = b;
    if (p->r > q->r) return -1;
    if (p->r < q->r) return  1;
    return 0;
}

static int dist_asc(const void * a, const void * b)
{
    const botcounter_t *p = a, *q = b;
    if (p->dist < q->dist) return -1;
    if (p->dist > q->dist) return  1;
    return 0;
}

static int64_t manh(const pos_t * const a, const pos_t * const b)
{
    return llabs(a->x - b->x) + llabs(a->y - b->y) + llabs(a->z - b->z);
}

static bool inrange(const bot_t * const ref, const bot_t * const test)
{
    return manh(&ref->p, &test->p) <= ref->r;
}

int main(void)
{
    // Part 1
    size_t n = read();
    qsort(bot, n, sizeof *bot, range_desc);
    int count = 1;
    for (size_t i = 1; i < n; ++i)
        count += inrange(&bot[0], &bot[i]);
    printf("Part 1: %d\n", count);  // part 1: 933

    // Part 2
    const pos_t origin = (pos_t){0, 0, 0};
    for (size_t i = 0; i < n; ++i) {
        int64_t d = manh(&bot[i].p, &origin);
        int64_t add = d > bot[i].r ? d - bot[i].r : 0, del = d + bot[i].r;
        line[i * 2]     = (botcounter_t){add, +1};  // count one more bot at the near end of its range
        line[i * 2 + 1] = (botcounter_t){del, -1};  // count one fewer bot at the far end of its range
    }
    qsort(line, n * 2, sizeof *line, dist_asc);
    size_t i = 0;
    while (line[i].count == 1)  // this only works because no counts of -1 until after maximum
        ++i;
    printf("Part 2: %lld\n", line[i - 1].dist);  // part 2: 70887840

    return 0;
}
