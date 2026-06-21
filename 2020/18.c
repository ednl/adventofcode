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
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
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
static const Term init = {.val = 0, .op = '+'};

static char input[FSIZE];
static Term stack1[STACK1];
static Term stack2[STACK2];
static int slen1, slen2;

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

    u64 sum1 = 0, sum2 = 0;
    Term term1 = init, term2 = init;
    for (const char *c = input; c != end; ++c)
        switch (*c) {  // cases ordered by value was fastest for me
            case '\n':
                sum1 += term1.val;
                term1 = init;
                while (slen2) {  // clean up stack with multiplications
                    const u64 tmp = term2.val;
                    term2 = stack2[--slen2];
                    if (term2.op == '+') term2.val += tmp; else term2.val *= tmp;
                }
                sum2 += term2.val;
                term2 = init;
                break;
            case ' ':  // skip
                break;
            case '(':  // push
                stack1[slen1++] = term1;
                term1 = init;
                stack2[slen2++] = mark;  // first add marker (op=0)
                stack2[slen2++] = term2;
                term2 = init;
                break;
            case ')':  // pop
                {
                    const u64 tmp = term1.val;
                    term1 = stack1[--slen1];
                    if (term1.op == '+') term1.val += tmp; else term1.val *= tmp;
                }
                while (stack2[--slen2].op) {  // remove marker last
                    const u64 tmp = term2.val;
                    term2 = stack2[slen2];
                    if (term2.op == '+') term2.val += tmp; else term2.val *= tmp;
                }
                break;
            case '*':  // mul, push for part 2
                term1.op = term2.op = '*';
                stack2[slen2++] = term2;
                term2 = init;
                c++;  // operator always +space, so c+=2
                break;
            case '+':  // add
                term1.op = term2.op = '+';
                c++;  // operator always +space, so c+=2
                break;
            default:
                {
                    const u64 tmp = *c & 15;
                    if (term1.op == '+') term1.val += tmp; else term1.val *= tmp;
                    if (term2.op == '+') term2.val += tmp; else term2.val *= tmp;
                }
        }
    printf("%"PRIu64" %"PRIu64"\n", sum1, sum2);  // 280014646144 9966990988262

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
