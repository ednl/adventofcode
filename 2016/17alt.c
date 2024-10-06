/**
 * Advent of Code 2016
 * Day 17: Two Steps Forward
 * https://adventofcode.com/2016/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 17alt.c mymd5.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 17alt.c mymd5.c ../startstoptimer.c
 * Get minimum runtime (input is 8-char string as command line argument):
 *     m=999999;for((i=0;i<1000;++i));do t=$(./a.out udskfozm|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : 44337 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)            : ? µs
 *     Raspberry Pi 4 (1.8 GHz)            : ? µs
 */

#include <stdio.h>   // printf
#include <stdlib.h>  // malloc, realloc, free
#include <string.h>  // memcpy
#include "mymd5.h"
#include "../startstoptimer.h"

#define N 4
#define M (N - 1)

static const char dir[] = "UDLR";
static const int dx[] = {0, 0, -1, 1};
static const int dy[] = {-1, 1, 0, 0};
static const size_t dirsize = sizeof dx / sizeof *dx;

static char *salt, *shortestpath;
static size_t saltlen, shortest = (size_t)-1, longest = 0;

static void walk(int x, int y, size_t len, char *path)
{
    char hex[33];
    if (x == M && y == M) {
        if (len < shortest) {
            shortest = len;
            shortestpath = realloc(shortestpath, len + 1);
            memcpy(shortestpath, path, len + 1);
        }
        if (len > longest)
            longest = len;
        return;
    }
    mymd5(path, hex);
    for (size_t i = 0; i < dirsize; ++i) {
        if (hex[i] > 'a') {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
                char *next = malloc(len + 2);
                memcpy(next, path, len);
                next[len] = dir[i];
                next[len + 1] = '\0';
                walk(nx, ny, len + 1, next);
                free(next);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    starttimer();
    if (argc != 2)
        return 1;
    saltlen = strlen(argv[1]);
    salt = malloc(saltlen + 1);
    memcpy(salt, argv[1], saltlen + 1);
    walk(0, 0, saltlen, salt);
    printf("Part 1: %s\n", shortestpath + saltlen);
    printf("Part 2: %zu\n", longest - saltlen);

    free(salt);
    free(shortestpath);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
