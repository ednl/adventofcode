/**
 * Advent of Code 2018
 * Day 20: A Regular Map
 * https://adventofcode.com/2018/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark on an iMac (Late 2013, 3.2 GHz quad-core Core i5 "Haswell"),
 * compiler Apple clang 12.0.0 with -O3 -march=native:
 *
 *     $ hyperfine -N -w 100 -r 500 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):       4.5 ms ±   0.1 ms    [User: 3.2 ms, System: 0.6 ms]
 *       Range (min … max):     4.4 ms …   5.3 ms    500 runs
 *
 * Benchmark on a Raspberry Pi 4, compiler Debian gcc 10.2.1-6 with -O3 -march=native
 * and the CPU in performance mode:
 *
 *     echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *     (to reset, replace performance with ondemand)
 *     /boot/config.txt: arm_boost=1, no overclock
 *
 *     $ hyperfine -N -w 50 -r 200 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):      15.3 ms ±   0.7 ms    [User: 13.7 ms, System: 1.4 ms]
 *       Range (min … max):    14.0 ms …  17.6 ms    200 runs
*/
#include <stdio.h>    // fopen, fclose, fgetc, printf, putchar
#include <string.h>   // memmove
#include <stdint.h>   // int8_t, int16_t
#include <stdbool.h>  // bool, true, false

#define STACKSIZE   256  // needed for my input: 225
#define ROOMCOUNT 16384  // needed for my input: 10000 (grid 100x100)

typedef struct {
    int8_t x, y;
} Pos;

typedef struct {
    Pos pos;
    int16_t dist;
} Room;

static Room stack[STACKSIZE];
static size_t stacksize;
// static size_t maxstacksize;

static Room room[ROOMCOUNT];
static size_t roomcount;

// Sort rooms by position
static int cmp_pos(const void *a, const void *b)
{
    const Pos p = ((const Room *)a)->pos, q = ((const Room *)b)->pos;
    if (p.x < q.x) return -1;
    if (p.x > q.x) return  1;
    if (p.y < q.y) return -1;
    if (p.y > q.y) return  1;
    return 0;
}

// Insert into sorted array or update minimal distance to position.
// Return false if failed, or not needed, to insert or update.
static bool binsert(const Room a)
{
    if (roomcount >= ROOMCOUNT) {
        putchar('b');  // increase ROOMCOUNT
        return false;
    }

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
        // if (stacksize > maxstacksize)
        //     maxstacksize = stacksize;
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
    FILE *f = fopen("../aocinput/2018-20-input.txt", "r");
    if (!f)
        return 1;
    int c;
    Room a = {0};
    binsert(a);
    while ((c = fgetc(f)) != '$')
        switch (c) {
            case '(': push(a);  break;
            case '|': peek(&a); break;
            case ')': pop(&a);  break;
            case 'W': --a.pos.x; ++a.dist; binsert(a); break;
            case 'N': --a.pos.y; ++a.dist; binsert(a); break;
            case 'E': ++a.pos.x; ++a.dist; binsert(a); break;
            case 'S': ++a.pos.y; ++a.dist; binsert(a); break;
        }
    fclose(f);
    // printf("maxstacksize=%zu roomcount=%zu\n", maxstacksize, roomcount);

    int max = 0, big = 0;
    for (size_t i = 0; i < roomcount; ++i) {
        if (room[i].dist > max) max = room[i].dist;
        if (room[i].dist >= 1000) ++big;
    }
    printf("Part 1: %d\n", max);  // 3810
    printf("Part 2: %d\n", big);  // 8615
    return 0;
}
