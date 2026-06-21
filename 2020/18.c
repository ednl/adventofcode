/**
 * Advent of Code 2020
 * Day 18: Operation Order
 * https://adventofcode.com/2020/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 14.2 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 76.4 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-18-input.txt"
#define FSIZE 24000  // needed for my input: 20422
#define STACK1 4   // needed for my input: 2
#define STACK2 16  // needed for my input: 12

typedef uint64_t u64;
typedef struct term {
    u64 val;
    char op;
} Term;

static const Term mark = {0};
static const Term init = {.op = '+'};

static char input[FSIZE];
static Term stack1[STACK1];
static Term stack2[STACK2];
static int len1, len2;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    u64 part1 = 0, part2 = 0;
    Term x1 = init, x2 = init;
    for (const char *c = input; c != end; ++c)
        switch (*c) {  // cases ordered by value was fastest for me
            case '\n':
                part1 += x1.val;
                x1 = init;
                while (len2) {  // clean up stack with multiplications
                    const u64 tmp = x2.val;
                    x2 = stack2[--len2];
                    if (x2.op == '+')
                        x2.val += tmp;
                    else
                        x2.val *= tmp;
                }
                part2 += x2.val;
                x2 = init;
                break;
            case ' ':  // skip
                break;
            case '(':  // push
                stack1[len1++] = x1;
                x1 = init;
                stack2[len2++] = mark;  // first add marker (op=0)
                stack2[len2++] = x2;
                x2 = init;
                break;
            case ')':  // pop
                {
                    const u64 tmp = x1.val;
                    x1 = stack1[--len1];
                    if (x1.op == '+')
                        x1.val += tmp;
                    else
                        x1.val *= tmp;
                }
                while (stack2[--len2].op) {  // remove marker last
                    const u64 tmp = x2.val;
                    x2 = stack2[len2];
                    if (x2.op == '+')
                        x2.val += tmp;
                    else
                        x2.val *= tmp;
                }
                break;
            case '*':  // mul, +push for part 2
                x1.op = x2.op = '*';
                stack2[len2++] = x2;
                x2 = init;
                c++;  // operator always +space, so c+=2
                break;
            case '+':  // add
                x1.op = x2.op = '+';
                c++;  // operator always +space, so c+=2
                break;
            default:  // ascii numerical digit
                {
                    const u64 tmp = *c & 15;
                    if (x1.op == '+')
                        x1.val += tmp;
                    else
                        x1.val *= tmp;
                    if (x2.op == '+')
                        x2.val += tmp;
                    else
                        x2.val *= tmp;
                }
        }
    printf("%"PRIu64" %"PRIu64"\n", part1, part2);  // 280014646144 9966990988262

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
