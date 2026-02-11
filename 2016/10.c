/**
 * Advent of Code 2016
 * Day 10: Balance Bots
 * https://adventofcode.com/2016/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Run program:
 *     ./a.out                  read input file from internal file name
 *     ./a.out < input.txt      read input file using redirected input
 *     cat input.txt | ./a.out  read input file using piped input
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  26 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-10-input.txt"
#define LINES 256   // needed for my input: 231
#define LINELEN 64  // needed for my input: 52+2=54

#define BOTS 256    // needed for my input: 210 (max=209)
#define OUTPUTS 32  // needed for my input: 21 (max=20)
#define QSIZE 16    // needed for my input: 10

// From puzzle description: values to find for part 1
#define CHIP0 17
#define CHIP1 61

typedef struct bot {
    int lo, hi;  // destination IDs (>=BOTS: output)
    int count, val[2];
} Bot;

// Queue of bot IDs to process when Bot::count=2
static int queue[QSIZE];
static int qhead, qtail, qlen;

static char input[LINES][LINELEN];
static Bot bot[BOTS];
static int output[OUTPUTS];
static int part1 = -1;  // not found yet

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
        if (b->count == 2) {  // ready to distribute
            enqueue(id);
            if (b->val[0] > b->val[1]) {  // swap if out of order
                const int tmp = b->val[0];
                b->val[0] = b->val[1];
                b->val[1] = tmp;
            }
            if (part1 == -1)
                if (b->val[0] == CHIP0 && b->val[1] == CHIP1)
                    part1 = id;
        }
    } else
        output[id - BOTS] = val;
}

int main(void)
{
    int lines = 0;
    if (isatty(fileno(stdin))) {
        // Read lines of input file from disk
        FILE *f = fopen(FNAME, "r");
        if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
        for (; lines < LINES && fgets(&input[lines][0], LINELEN, f); ++lines);
        fclose(f);
    } else
        // Read lines of input or example file from pipe or redirected stdin
        for (; lines < LINES && fgets(&input[lines][0], LINELEN, stdin); ++lines);

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
        // Distribute chip values to other bots or outputs
        recv(b->lo, b->val[0]);
        recv(b->hi, b->val[1]);
        b->count = 0;
    }
    printf("%d %d\n", part1, output[0] * output[1] * output[2]);  // 147 55637

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
