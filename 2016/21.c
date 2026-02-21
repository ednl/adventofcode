/**
 * Advent of Code 2016
 * Day 21: Scrambled Letters and Hash
 * https://adventofcode.com/2016/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 21.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 21.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 4.08 µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>   // getline
#include <stdlib.h>  // free
#include <string.h>  // memcpy, strcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2016-21-example.txt"
    #define LINES 8
    #define PWD "abcde"
    #define LEN 5
#else
    #define FNAME "../aocinput/2016-21-input.txt"
    #define LINES 100
    #define PWD "abcdefgh"
    #define PW2 "fbgdceah"
    #define LEN 8
#endif

typedef enum opcode {
    NOP, MOVU, MOVD, SWAPI, SWAPC, REV, ROTC, ROTR
} Opcode;

typedef struct cmd {
    Opcode op;
    int x, y;
} Cmd;

static Cmd algo[LINES];
static char pwd[] = PWD;

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "FIle not found: "FNAME"\n");
        return 0;  // zero lines read
    }
    int n = 0;
    char *line = NULL;
    size_t size;
    ssize_t linelen;
    for (; n < LINES && (linelen = getline(&line, &size, f)) > 0; ++n) {
        Opcode op = NOP;
        int x = 0, y = 0;
        if (line[0] == 'm') {         // move position X to position Y
            x = line[14] - '0';
            y = line[28] - '0';
            op = x < y ? MOVU : MOVD;
        } else if (line[0] == 's') {  // swap ...
            if (line[5] == 'p') {     // swap position X with position Y
                op = SWAPI;
                x = line[14] - '0';
                y = line[30] - '0';
            } else {                  // swap letter X with letter Y
                op = SWAPC;
                x = line[12];
                y = line[26];
            }
        } else if (line[1] == 'e') {  // reverse positions X through Y
            op = REV;
            x = line[18] - '0';
            y = line[28] - '0';
        } else {                      // rotate ...
            switch (line[7]) {
                case 'b': op = ROTC; x = line[35];               break;  // rotate based on position of letter X
                case 'l': op = ROTR; y = LEN - (line[12] - '0'); break;  // rotate left  X steps
                case 'r': op = ROTR; y = line[13] - '0';         break;  // rotate right X steps
            }
        }
        algo[n] = (Cmd){op, x, y};
    }
    fclose(f);
    free(line);
    return n;
}

static void scramble(int n)
{
    char t, tmp[LEN];
    int x, y;
    for (int i = 0; i < n; ++i) {
        Cmd *const cmd = &algo[i];
        switch (cmd->op) {
            case NOP: break;
            case MOVU:
                t = pwd[cmd->x];
                for (int k = cmd->x; k < cmd->y; ++k)
                    pwd[k] = pwd[k + 1];
                pwd[cmd->y] = t;
                break;
            case MOVD:
                t = pwd[cmd->x];
                for (int k = cmd->x; k > cmd->y; --k)
                    pwd[k] = pwd[k - 1];
                pwd[cmd->y] = t;
                break;
            case SWAPI:
                t = pwd[cmd->x];
                pwd[cmd->x] = pwd[cmd->y];
                pwd[cmd->y] = t;
                break;
            case SWAPC:
                x = 0;
                y = 0;
                while (pwd[x] != cmd->x) ++x;
                while (pwd[y] != cmd->y) ++y;
                pwd[x] = (char)cmd->y;
                pwd[y] = (char)cmd->x;
                break;
            case REV:
                x = cmd->x;
                y = cmd->y;
                while (x < y) {
                    t = pwd[x];
                    pwd[x++] = pwd[y];
                    pwd[y--] = t;
                }
                break;
            case ROTC:  // rotate based on position of letter X
                x = 0;
                while (pwd[x] != cmd->x) ++x;
                cmd->y = (1 + x + (x >= 4)) % LEN;
                // Fall through
            case ROTR:  // rotate right Y steps
                if (cmd->y) {
                    for (int k = 0; k < LEN; ++k)
                        tmp[(k + cmd->y) % LEN] = pwd[k];
                    memcpy(pwd, tmp, LEN);
                }
                break;
        }
    }
}

#if !EXAMPLE
static void unscramble(int n)
{
    char t, tmp[LEN];
    int x, y;
    for (int i = n - 1; i >= 0; --i) {
        Cmd *cmd = &algo[i];
        switch (cmd->op) {
            case NOP: break;
            case MOVU:  // move position X to position Y (X < Y)
                t = pwd[cmd->y];
                for (int k = cmd->y; k > cmd->x; --k)
                    pwd[k] = pwd[k - 1];
                pwd[cmd->x] = t;
                break;
            case MOVD:  // move position X to position Y (X > Y)
                t = pwd[cmd->y];
                for (int k = cmd->y; k < cmd->x; ++k)
                    pwd[k] = pwd[k + 1];
                pwd[cmd->x] = t;
                break;
            case SWAPI:  // swap position X with position Y
                t = pwd[cmd->x];
                pwd[cmd->x] = pwd[cmd->y];
                pwd[cmd->y] = t;
                break;
            case SWAPC:  // swap letter X with letter Y
                x = 0;
                y = 0;
                while (pwd[x] != cmd->x) ++x;
                while (pwd[y] != cmd->y) ++y;
                pwd[x] = (char)cmd->y;
                pwd[y] = (char)cmd->x;
                break;
            case REV:  // reverse positions X through Y
                x = cmd->x;
                y = cmd->y;
                while (x < y) {
                    t = pwd[x];
                    pwd[x++] = pwd[y];
                    pwd[y--] = t;
                }
                break;
            case ROTC:  // rotate based on position of letter X
                for (x = 0; pwd[x] != cmd->x; ++x);
                cmd->y = (!x || (x & 1) ? (x >> 1) + 1 : (x >> 1) + 5) % LEN;
                /* Fall through */
            case ROTR:  // rotate right Y steps
                if (cmd->y) {
                    for (int k = 0; k < LEN; ++k)
                        tmp[k] = pwd[(k + cmd->y) % LEN];
                    memcpy(pwd, tmp, LEN);
                }
                break;
        }
    }
}
#endif

int main(void)
{
    int n = parse();

#ifdef TIMER
    starttimer();
#endif

    scramble(n);
    printf("Part 1: %s\n", pwd);  // bfheacgd
#if !EXAMPLE
    strcpy(pwd, PW2);
    unscramble(n);
    printf("Part 2: %s\n", pwd);  // gcehdbfa
#endif

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
