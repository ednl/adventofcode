/**
 * Advent of Code 2017
 * Day 12: Digital Plumber
 * https://adventofcode.com/2017/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -Wno-multichar -DTIMER 12.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 14 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-12-input.txt"
#define N 2000  // 2000 "programs" (nodes)
#define M 6     // max 6 "pipes" (edges) from any program

typedef struct program {
    int group;
    int pipe[M];
} Program;

static Program program[N];

static int connect(int id, int group)
{
    Program *const p = &program[id];  // convenience pointer
    if (p->group)
        return 0;
    p->group = group;
    int count = 1 + connect(p->pipe[0], group);
    for (int j = 1; j < M && p->pipe[j]; ++j)
        count += connect(p->pipe[j], group);
    return count;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 1;
    }
    for (int n = 0, id, pipe; n < N && fscanf(f, "%d <-> %d", &id, &pipe) == 2; ++n) {
        program[id].pipe[0] = pipe;
        for (int j = 1; j < M && fscanf(f, ", %d", &pipe) == 1; ++j)
            program[id].pipe[j] = pipe;
    }
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Program ID 0, group 1
    printf("%d\n", connect(0, 1));  // 145

    // All other programs not in a group yet
    int group = 1;
    for (int n = 1; n < N; ++n)
        if (!program[n].group)
            connect(n, ++group);
    printf("%d\n", group);  // 207

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
