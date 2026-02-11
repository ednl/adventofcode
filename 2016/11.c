/**
 * Advent of Code 2016
 * Day 11: Radioisotope Thermoelectric Generators
 * https://adventofcode.com/2016/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  3.64 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ?    ms
 *     Raspberry Pi 5 (2.4 GHz)      : 15.7  ms
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <string.h>  // memmove
#include <stdint.h>  // uint8_t, uint32_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif
#define DEBUG 0

#define FLOORS     4  // floors numbered [0..3] = 2 bits per number
#define MAXELMS    7  // if more than 7, hash won't fit in 32 bits
#define QSIZE    256  // actually needed for my input: 9, 87, 190
#define QMASK (QSIZE - 1)  // QSIZE must be power of 2 => mask of all ones
#define SSIZE   4096  // actually needed for my input: 50, 1150, 3869

typedef struct {
    uint8_t generator, microchip;  // floor numbers
} Pair;

// lowest = lowest floor number with any device
// pair[0..diff-1]: gen and mic are on different floors
// pair[diff..elms-1]: gen and mic are on the same floor
typedef struct {
    Pair pair[MAXELMS];
    uint8_t elevator, step, lowest, diff;
} State;

typedef struct {
    size_t len, pop, ins;
    State q[QSIZE];
} queue_t;

static queue_t queue;
#if DEBUG
static size_t qlen;
#endif
static uint32_t seen[SSIZE];
static size_t seenlen;

static int cmp_pair(const void *p, const void *q)
{
    const Pair *a = p, *b = q;
    // First pairs that are NOT on the same floor, then pairs that ARE on the same floor
    if (a->generator != a->microchip && b->generator == b->microchip) return -1;
    if (a->generator == a->microchip && b->generator != b->microchip) return  1;
    // Within each category, sort by generator then microchip
    if (a->generator < b->generator) return -1;
    if (a->generator > b->generator) return  1;
    if (a->microchip < b->microchip) return -1;
    if (a->microchip > b->microchip) return  1;
    return 0;
}

static State norm(const State s, const int elms)
{
    State t = s;
    qsort(t.pair, (size_t)elms, sizeof (Pair), cmp_pair);

    uint8_t i = 0;
    for (; i < elms && t.pair[i].generator != t.pair[i].microchip; ++i);
    t.diff = (uint8_t)i;

    t.lowest = t.elevator;
    for (i = 0; t.lowest && i < t.diff; ++i) {
        if (t.pair[i].generator < t.lowest) t.lowest = t.pair[i].generator;
        if (t.pair[i].microchip < t.lowest) t.lowest = t.pair[i].microchip;
    }
    for (i = t.diff; t.lowest && i < elms; ++i)
        if (t.pair[i].generator < t.lowest) t.lowest = t.pair[i].generator;

    return t;
}

static bool islegal(const State s, const int elms)
{
    for (int i = 0; i < s.diff; ++i) {  // check all single microchips
        const int m = s.pair[i].microchip;
        for (int j = 0; j < elms; ++j)  // for *all *other generators
            if (i != j && m == s.pair[j].generator)
                return false;
    }
    return true;
}

// State hash value from floor numbers of elevator + all devices (sorted).
// Elevator + max. 7 elements, 2 bits per element = 30 bits.
static uint32_t hash(const State s, const int elms)
{
    uint32_t h = s.elevator;
    for (int i = 0; i < elms; ++i)
        h = (h << 4) | ((uint32_t)s.pair[i].generator << 2) | ((uint32_t)s.pair[i].microchip);
    return h;
}

// Insert new hash value and return true.
// Return false if value already present or can't be inserted.
static bool binsert(const uint32_t k, uint32_t *a, size_t *len, const size_t size)
{
    if (!a || !len || *len >= size) {
        putchar('s');  // major error
        return false;
    }
    if (!*len || k > a[*len - 1]) {
        a[(*len)++] = k;
        return true;
    }
    if (k < a[0]) {
        memmove(a + 1, a, (*len)++ * sizeof *a);
        a[0] = k;
        return true;
    }
    size_t l = 0, r = *len - 1;
    if (k == a[l] || k == a[r])
        return false;
    // Now always true: a[l] < k < a[r]
    while (r - l != 1) {
        size_t m = ((l + r) >> 1);
        if      (k > a[m]) l = m;
        else if (k < a[m]) r = m;
        else               return false;
    }
    memmove(a + r + 1, a + r, ((*len)++ - r) * sizeof *a);
    a[r] = k;
    return true;
}

// Dequeue = pop off the tail of the queue
static bool deq(queue_t *const q, State *const val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    q->pop &= QMASK;  // QSIZE must be power of 2 so QMASK is all ones
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(queue_t *const q, const State val)
{
    if (!q || q->len == QSIZE) {
        putchar('q');  // major error
        return false;
    }
    q->len++;
#if DEBUG
    if (q->len > qlen)
        qlen = q->len;
#endif
    q->q[q->ins++] = val;
    q->ins &= QMASK;  // QSIZE must be power of 2 so QMASK is all ones
    return true;
}

static bool marktodo(const State s, const int elms)
{
    return binsert(hash(s, elms), seen, &seenlen, SSIZE) && enq(&queue, s);
}

#if DEBUG
static void show(const State s, const int elms, const bool indent)
{
    if (indent)
        putchar('\t');
    printf("%2d: E%d", s.step, s.elevator + 1);
    for (int i = 0; i < elms; ++i)
        printf(" (%d,%d)", s.pair[i].generator + 1, s.pair[i].microchip + 1);
    putchar('\n');
}
#endif

static uint8_t solve(const int part)
{
    static const int elmcount[] = {2, 5, 7};
    static const State input[] = {
        (State){{{1,0},{2,0}                              },0,0,0,0},  // example
        (State){{{0,1},{2,2},{2,2},{0,1},{0,0}            },0,0,0,0},  // my puzzle input
        (State){{{0,1},{2,2},{2,2},{0,1},{0,0},{0,0},{0,0}},0,0,0,0}   // add two pairs on first floor
    };
    static uint8_t *here[MAXELMS * 2];

    // Init
    const int elms = elmcount[part];
    State s = norm(input[part], elms);

    // Reset
    seenlen = queue.len = queue.pop = queue.ins = 0;

    // BFS with lots of pruning
    marktodo(s, elms);
    while (deq(&queue, &s)) {
#if DEBUG
        if (part == 0)
            show(s, elms, false);
#endif
        if (s.lowest == FLOORS - 1) {
#if DEBUG
            if (part == 0)
                putchar('\n');
            printf("seen=%zu qlen=%zu\n", seenlen, qlen);
#endif
            return s.step;
        }

        // Which devices are on the same floor as the elevator?
        int count = 0;
        for (int i = 0; i < s.diff; ++i) {
            if      (s.pair[i].generator == s.elevator) here[count++] = &s.pair[i].generator;
            else if (s.pair[i].microchip == s.elevator) here[count++] = &s.pair[i].microchip;
        }
        for (int i = s.diff; i < elms; ++i)
            if (s.pair[i].generator == s.elevator) {
                here[count++] = &s.pair[i].generator;
                here[count++] = &s.pair[i].microchip;
            }

        s.step++;
        int branch = 0;

        // Always try 2 up
        if (count >= 2 && s.elevator < FLOORS - 1) {
            s.elevator++;
            for (int i = 0; i < count - 1; ++i) {
                (*here[i])++;
                for (int j = i + 1; j < count; ++j) {
                    (*here[j])++;
                    State t = norm(s, elms);
                    if (islegal(t, elms) && marktodo(t, elms)) {
                        ++branch;
#if DEBUG
                        if (part == 0) {
                            putchar('\t');
                            show(t, elms, true);
                        }
#endif
                    }
                    (*here[j])--;
                }
                (*here[i])--;
            }
            s.elevator--;
        }

        // Always try 1 down
        if (count >= 1 && s.elevator > s.lowest) {
            s.elevator--;
            for (int i = 0; i < count; ++i) {
                (*here[i])--;
                State t = norm(s, elms);
                if (islegal(t, elms) && marktodo(t, elms)) {
                    ++branch;
#if DEBUG
                    if (part == 0) {
                        putchar('\t');
                        show(t, elms, true);
                    }
#endif
                }
                (*here[i])++;
            }
            s.elevator++;
        }

        // Only try 1 up if nothing else
        if (!branch && count >= 1 && s.elevator < FLOORS - 1) {
            s.elevator++;
            for (int i = 0; i < count; ++i) {
                (*here[i])++;
                State t = norm(s, elms);
                if (islegal(t, elms) && marktodo(t, elms)) {
                    ++branch;
#if DEBUG
                    if (part == 0) {
                        putchar('\t');
                        show(t, elms, true);
                    }
#endif
                }
                (*here[i])--;
            }
            s.elevator--;
        }

        // Only try 2 down if nothing else
        if (!branch && count >= 2 && s.elevator > s.lowest) {
            s.elevator--;
            for (int i = 0; i < count - 1; ++i) {
                (*here[i])--;
                for (int j = i + 1; j < count; ++j) {
                    (*here[j])--;
                    State t = norm(s, elms);
                    if (islegal(t, elms) && marktodo(t, elms)) {
                        ++branch;
#if DEBUG
                        if (part == 0) {
                            putchar('\t');
                            show(t, elms, true);
                        }
#endif
                    }
                    (*here[j])++;
                }
                (*here[i])++;
            }
            s.elevator++;
        }
    }
    return 0;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    for (int i = 1; i < 3; ++i)
        printf("Part %d: %hhu\n", i, solve(i));  // 11, 31, 55

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
