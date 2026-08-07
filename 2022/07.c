/**
 * Advent of Code 2022
 * Day 7: No Space Left On Device
 * https://adventofcode.com/2022/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.35 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 6.42 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2022-07-example.txt"
    #define FSIZE 256  // needed for example: 192
    #define NAME  1    // min number of chars in file/dir names
    #define DEPTH 3    // directory recursion depth
    #define DIRS  4    // max number of subdirs
#else
    #define FNAME "../aocinput/2022-07-input.txt"
    #define FSIZE (8192 + 4096)  // needed for my input: 11301
    #define NAME   3  // min number of chars in file/dir names
    #define DEPTH 15  // directory recursion depth (needed for my input: 12)
    #define DIRS 255  // max number of subdirs (needed for my input: 189)
#endif
#define MAX1       (100 * 1000)  // part 1: max dir size to consider
#define DISK (70 * 1000 * 1000)  // total disk size
#define NEED (30 * 1000 * 1000)  // part 2: free space needed

// Size of 'working' can be much smaller (DEPTH vs. DIRS)
// so should be dynamic size or different types
typedef struct stack {
    int sp;
    int data[DIRS];
} Stack;

static char input[FSIZE];
static Stack working, dirsize;

// Assume stack is never full
static void push(Stack *const stack, const int val)
{
    stack->data[stack->sp++] = val;
}

// Assume stack is never empty
static int pop(Stack *const stack)
{
    return stack->data[--stack->sp];
}

static bool popchk(Stack *const restrict stack, int *const restrict val)
{
    if (stack->sp) {
        *val = stack->data[--stack->sp];
        return true;
    }
    return false;
}

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s != ' ')  // all numbers are followed by space
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static void skipline(const char **s, const int minchars)
{
    for ((*s) += minchars; **s != '\n'; (*s)++);  // skip until newline
    (*s)++;  // skip newline
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    working.sp = dirsize.sp = 0;
#endif

    // "$ cd abc" : go down
    // "$ cd .."  : go up
    // "$ ls"     : ignore
    // "dir abc"  : ignore
    // "123 x.y"  : add size
    int cursize = 0;
    for (const char *c = input; *c; ) {
        if (*c == '$') {                       // command?
            if (c[2] == 'c') {                 // change directory?
                if (c[5] == '.') {             // go up?
                    push(&dirsize, cursize);   // end of directory, save final size
                    cursize += pop(&working);  // add size of parent (= add to size of parent)
                    c += 8;                    // skip "$ cd .." +newline
                } else {                       // go down
                    push(&working, cursize);   // save current dir size
                    cursize = 0;               // reset size for subdir
                    skipline(&c, 5 + NAME);    // skip "$ cd abc" and more +newline
                }
            } else                             // list directory
                c += 5;                        // skip "$ ls" +newline
        } else if (*c != 'd') {                // file entry?
            cursize += parseint(&c);           // add+skip size
            skipline(&c, 1 + NAME);            // skip " abc" and more +newline
        } else                                 // directory entry?
            skipline(&c, 4 + NAME);            // skip "dir abc" and more +newline
    }

    // Clean up stack of working directory sizes
    for (int x; popchk(&working, &x); ) {
        push(&dirsize, cursize);  // save current size as final dir size
        cursize += x;             // add current and parent sizes
    }

    // Part 2: directory size to delete to get at least NEED free space
    const int minsize = cursize - DISK + NEED;

    int part1 = 0, part2 = DISK;
    for (int x; popchk(&dirsize, &x); ) {
        if (x <= MAX1)
            part1 += x;
        if (x >= minsize && x < part2)
            part2 = x;
    }
    printf("%d %d\n", part1, part2);  // 1845346 3636703

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
