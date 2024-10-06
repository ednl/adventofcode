/**
 * Advent of Code 2023
 * Day 25: Snowverload
 * https://adventofcode.com/2023/day/25
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fgets, printf
#include <stdlib.h>  // qsort

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-25-example.txt"
#define N 13  // lines
#define M 46  // nodes
#else
#define NAME "../aocinput/2023-25-input.txt"
#define N 1264  // lines
#define M 4739  // nodes
#endif
#define E (M - N)  // edges
#define L ('z' - 'b' + 1)
#define L3 (L * L * L)

static int edge[E];

static int asc(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static int hash(const char *s)
{
    int id = 0;
    for (const char *const end = s + 3; s != end; ++s)
        id = id * L + (*s - 'b');
    return id;
}

#if EXAMPLE || defined(DEBUG)
static void unhash(int id, char *const s)
{
    for (char *t = s + 3; t != s; id /= L)
        *(--t) = 'b' + id % L;
    *(s + 3) = '\0';
}
#endif

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[40];
    int e = 0;
    while (fgets(buf, sizeof buf, f)) {
        const int a = hash(buf);
        const char *s = buf + 4;
        while (e < E && *s == ' ') {
            const int b = hash(s + 1);
            edge[e++] = a < b ? a * L3 + b : b * L3 + a;
            s += 4;
        }
    }
    fclose(f);

    qsort(edge, (size_t)e, sizeof *edge, asc);
    #if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < e; ++ i) {
        char s[4], t[4];
        unhash(edge[i] / L3, s);
        unhash(edge[i] % L3, t);
        printf("%4d: %s-%s\n", i, s, t);
    }
    #endif

    return 0;
}
