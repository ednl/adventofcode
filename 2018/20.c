/**
 * Advent of Code 2018
 * Day 20: A Regular Map
 * https://adventofcode.com/2018/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 20.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 20.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, includes all parsing but not reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.40 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : 2.17 ms
 *     Raspberry Pi 5 (2.4 GHz)      : 3.88 ms
 */

#include <stdio.h>    // fopen, fclose, fgetc, printf, putchar
#include <string.h>   // memmove
#include <stdint.h>   // int8_t, int16_t
#include <stdbool.h>  // bool, true, false
#include "../startstoptimer.h"

#define FNAME "../aocinput/2018-20-input.txt"
#define FSIZE     16384  // needed for my input: 14185
#define STACKSIZE   256  // needed for my input: 225
#define ROOMCOUNT 16384  // needed for my input: 10000 (grid 100x100)
#define BIG        1000  // distance >=1000 is big

typedef struct pos {
    int8_t x, y;
} Pos;

typedef struct room {
    Pos pos;
    int16_t dist;
} Room;

static char input[FSIZE];
static Room stack[STACKSIZE];
static size_t stacksize;
// static size_t maxstacksize;

static Room room[ROOMCOUNT];
static size_t roomcount;

// Sort rooms by position
static int cmp_pos(const void *a, const void *b)
{
    const Pos *const p = &((const Room *)a)->pos;
    const Pos *const q = &((const Room *)b)->pos;
    if (p->x < q->x) return -1;
    if (p->x > q->x) return  1;
    if (p->y < q->y) return -1;
    if (p->y > q->y) return  1;
    return 0;
}

// Insert into sorted array or update minimal distance to position.
// Return false if failed, or not needed, to insert or update.
static bool binsert(const Room a)
{
    // if (roomcount >= ROOMCOUNT) {
    //     putchar('b');  // increase ROOMCOUNT
    //     return false;
    // }

    // First element in empty array.
    if (!roomcount) {
        room[roomcount++] = a;
        return true;
    }

    size_t r = roomcount - 1;
    const int cmpR = cmp_pos(&a, &room[r]);
    if (cmpR == 1) {
        // Insert as new last element.
        room[roomcount++] = a;
        return true;
    }
    if (cmpR == 0) {
        if (a.dist >= room[r].dist)
            return false;
        room[r].dist = a.dist;  // update
        return true;
    }

    size_t l = 0;
    const int cmpL = cmp_pos(&a, &room[l]);
    if (cmpL == -1) {
        // Insert as new first element.
        memmove(room + 1, room, roomcount++ * sizeof *room);
        room[l] = a;
        return true;
    }
    if (cmpL == 0) {
        if (a.dist >= room[l].dist)
            return false;
        room[l].dist = a.dist;  // update
        return true;
    }

    // Now always true: room[l].pos < a.pos < room[r].pos
    while (r - l != 1) {
        size_t m = ((l + r) >> 1);
        const int cmpM = cmp_pos(&a, &room[m]);
        if      (cmpM ==  1) l = m;
        else if (cmpM == -1) r = m;
        else {
            if (a.dist >= room[m].dist)
                return false;
            room[m].dist = a.dist;  // update
            return true;
        }
    }

    // Insert between room[l] and room[r]
    memmove(room + r + 1, room + r, (roomcount++ - r) * sizeof *room);
    room[r] = a;
    return true;
}

// Push current pos/dist onto stack.
static bool push(const Room a)
{
    if (stacksize < STACKSIZE) {
        stack[stacksize++] = a;
        return true;
    }
    putchar('u');  // increase STACKSIZE
    return false;
}

// Retrieve last pos/dist from stack but leave it there.
static bool peek(Room * const a)
{
    if (stacksize) {
        *a = stack[stacksize - 1];
        return true;
    }
    putchar('e');  // bad input
    return false;
}

// Pop last pos/dist from stack.
static bool pop(Room * const a)
{
    if (stacksize) {
        *a = stack[--stacksize];
        return true;
    }
    putchar('o');  // bad input
    return false;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) {
        fputs("File not found: "FNAME, stderr);
        return 1;
    }
    fread(input, 1, FSIZE, f);
    fclose(f);

    starttimer();

    Room a = {0};
    binsert(a);
    for (const char *c = input; *c; ++c)
        switch (*c) {
            case '(': push(a);  break;
            case ')': pop(&a);  break;
            case 'E': a.pos.x++; a.dist++; binsert(a); break;
            case 'N': a.pos.y--; a.dist++; binsert(a); break;
            case 'S': a.pos.y++; a.dist++; binsert(a); break;
            case 'W': a.pos.x--; a.dist++; binsert(a); break;
            case '|': peek(&a); break;
        }

    int max = 0, big = 0;
    for (size_t i = 0; i < roomcount; ++i) {
        if (room[i].dist >  max) max = room[i].dist;
        if (room[i].dist >= BIG) big++;
    }
    printf("%d\n", max);  // part 1: 3810
    printf("%d\n", big);  // part 2: 8615
    printf("Time: %.0f us\n", stoptimer_us());
}
