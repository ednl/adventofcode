/**
 * Advent of Code 2022
 * Day 5: Supply Stacks
 * https://adventofcode.com/2022/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.85 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 10.4  µs
 */

#include <stdio.h>
#include <string.h>  // memset, memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-05-input.txt"
#define FSIZE (8192 + 2048)  // needed for my input: 9921
#define STACKS 10  // stacks in input file numbered 1..9 (0 unused)
#define HEIGHT 8   // initial max stack height in input file, needed for my input: 8
#define SSIZE  60  // max crates per stack = 9 x 8 - empty spots, needed for my input: 56 (effectively: 49)
#define COLS ((STACKS - 1) * 4)  // line length of table at start of input file = 9 stacks (1-based), 4 chars per crate

typedef struct stack {
    int sp;
    char data[SSIZE];
} Stack;

static char input[FSIZE];
static Stack stack1[STACKS], stack2[STACKS];

static void move1(Stack *const restrict dst, Stack *const restrict src, int count)
{
    while (count--)
        dst->data[dst->sp++] = src->data[--src->sp];
}

static void move2(Stack *const restrict dst, Stack *const restrict src, const int count)
{
    src->sp -= count;
    memcpy(&dst->data[dst->sp], &src->data[src->sp], count);
    dst->sp += count;
}

static void sol(const Stack *const st)
{
    for (int i = 1; i < STACKS; ++i)
        putchar(st[i].data[st[i].sp - 1]);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char (*table)[COLS] = (char (*)[COLS])input;  // overlay variable for easy access, cast to avoid warning
    for (int i = 1; i < STACKS; ++i) {
        const int col = (i - 1) * 4 + 1;  // make 0-based, 4 chars per crate, content offset is 1
        int empty = 0;
        while (table[empty][col] == ' ')
            ++empty;
        stack1[i].sp = HEIGHT - empty;
        for (int j = 0; j < HEIGHT - empty; ++j)
            stack1[i].data[j] = table[HEIGHT - 1 - j][col];
    }
    memcpy(stack2, stack1, sizeof stack1);  // parts 1 & 2 start with the same stacks

    for (const char *c = input + 4 * (STACKS - 1) * (HEIGHT + 1) + 1; *c; ) {
        int src, dst, count;
        if (*(c + 6) == ' ') {  // single digit number of crates? (most prevalent)
            count = *(c + 5) & 15;
            src = *(c + 12) & 15;
            dst = *(c + 17) & 15;
            c += 19;  // "move 2 from 8 to 1" +newline
        } else {
            count = *(c + 5) * 10 + *(c + 6) - 11 * '0';
            src = *(c + 13) & 15;
            dst = *(c + 18) & 15;
            c += 20;  // "move 10 from 2 to 7" +newline
        }
        move1(&stack1[dst], &stack1[src], count);
        move2(&stack2[dst], &stack2[src], count);
    }

    sol(stack1);  // PSNRGBTFT
    putchar(' ');
    sol(stack2);  // BNTZFPMMW
    putchar('\n');

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
