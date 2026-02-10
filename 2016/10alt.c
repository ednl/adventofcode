#include <stdio.h>
// #include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-10-input.txt"
#define LINES 256
#define LINELEN 64

#define BOTS 256
#define OUTPUTS 32
#define QSIZE BOTS
#define CHIP0 17
#define CHIP1 61

typedef struct bot {
    int lo, hi;  // destination IDs (>=BOTS: output)
    int count, val[2];
} Bot;

// Queue of bot IDs to process (.count=2)
static int queue[QSIZE];
static int qhead, qtail, qlen;

static char input[LINES][LINELEN];
static Bot bot[BOTS];
static int output[OUTPUTS];
static int part1 = -1;

static void enqueue(const int val)
{
    if (qlen == QSIZE)
        return;
    queue[qhead++] = val;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
}

static int dequeue(void)
{
    if (qlen == 0)
        return -1;
    qlen--;
    const int i = qtail;
    if (++qtail == QSIZE)
        qtail = 0;
    return queue[i];
}

static void recv(const int id, const int val)
{
    if (id < BOTS) {
        Bot *const b = &bot[id];
        b->val[b->count++] = val;
        if (b->count == 2) {
            enqueue(id);
            if (b->val[0] > b->val[1]) {
                const int tmp = b->val[0];
                b->val[0] = b->val[1];
                b->val[1] = tmp;
            }
            if (b->val[0] == CHIP0 && b->val[1] == CHIP1)
                part1 = id;
        }
    } else
        output[id - BOTS] = val;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    int lines = 0;
    for (; lines < LINES && fgets(&input[lines][0], LINELEN, f); ++lines);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    for (int line = 0; line < lines; ++line) {
        const char *const s = &input[line][0];
        int x, y, z;
        char lo[8], hi[8];
        if (s[0] == 'v') {
            if (sscanf(s + 6, "%d goes to bot %d", &x, &y) == 2)
                recv(y, x);
            continue;
        }
        if (sscanf(s, "bot %d gives low to %7s %d and high to %7s %d", &x, lo, &y, hi, &z) == 5) {
            Bot *const b = &bot[x];
            b->lo = lo[0] == 'b' ? y : y + BOTS;
            b->hi = hi[0] == 'b' ? z : z + BOTS;
        }
    }
    while (qlen) {
        Bot *const b = &bot[dequeue()];
        recv(b->lo, b->val[0]);
        recv(b->hi, b->val[1]);
        b->count = 0;
    }
    printf("%d %d\n", part1, output[0] * output[1] * output[2]);  // 147 55637

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
