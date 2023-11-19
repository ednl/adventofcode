#include <stdio.h>    // printf
#include <string.h>   // strlen, memcpy
#include <stdbool.h>  // bool
#include "mymd5.h"

#define N 4         // dimensions = 4x4
#define M (N - 1)   // max coordinate = 3
#define PSIZE 1008  // max strlen of input+path = 1007 (1008 - one NUL char) => sizeof (state_t) = 1024
#define QSIZE 512   // max number of paths queued = 512 (max number detected = 293 for example 3)

typedef struct {
    int x, y;
    size_t len;
    char path[PSIZE];
} state_t;

typedef struct {
    size_t len, pop, ins;
    state_t q[QSIZE];
} queue_t;

static const char dir[] = "UDLR";
static const int dx[] = {0, 0, -1, 1};
static const int dy[] = {-1, 1, 0, 0};
static const size_t dirsize = sizeof dx / sizeof *dx;
static queue_t queue;

// Dequeue = pop off the tail of the queue
static bool deq(queue_t *q, state_t * const val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    if (q->pop == QSIZE)
        q->pop = 0;
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(queue_t *q, const state_t * const val)
{
    if (!q || q->len == QSIZE)
        return false;
    q->len++;
    q->q[q->ins++] = *val;
    if (q->ins == QSIZE)
        q->ins = 0;
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    size_t baselen = strlen(argv[1]);
    if (baselen >= PSIZE)
        return 2;

    bool first = true;
    size_t longest = 0;
    state_t s = {0};
    s.len = baselen;
    memcpy(s.path, argv[1], baselen + 1);  // include NUL terminator
    enq(&queue, &s);

    while (deq(&queue, &s)) {
        if (!(s.x == M && s.y == M)) {
            // Not reached vault yet, check open doors
            if (s.len < PSIZE - 1) {  // room for 1 extra char + NUL?
                char *c = mymd5_tostr(s.path);
                for (size_t i = 0; i < dirsize; ++i) {
                    if (c[i] > 'a') {
                        state_t t = s;
                        t.x += dx[i];
                        t.y += dy[i];
                        if (t.x >= 0 && t.x < N && t.y >= 0 && t.y < N) {
                            t.path[t.len++] = dir[i];  // add new direction to path
                            t.path[t.len] = '\0';
                            if (!enq(&queue, &t))
                                puts("***** ERROR ***** queue overflow");
                        }
                    }
                }
            } else
                puts("***** ERROR ***** string overflow");
        } else {
            // Reached vault, go no further on this path
            if (first) {
                // Ex1   : "ihgpwlah" = DDRRRD
                // Ex2   : "kglvqrro" = DDUDRLRRUDRD
                // Ex3   : "ulqzkmiv" = DRURDRUDDLLDLUURRDULRLDUUDDDRR
                // Input : "udskfozm" = DDRLRRUDDR
                printf("Part 1: %s\n", s.path + baselen);
                first = false;
            }
            // FIFO queue should mean: longest comes last
            if (s.len > longest)
                longest = s.len;
        }
    }
    // Ex1   : "ihgpwlah" = 370
    // Ex2   : "kglvqrro" = 492
    // Ex3   : "ulqzkmiv" = 830
    // Input : "udskfozm" = 556
    printf("Part 2: %zu\n", longest >= baselen ? longest - baselen : 0);
    return 0;
}
