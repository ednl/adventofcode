/**
 * Advent of Code 2023
 * Day 23: A Long Walk
 * https://adventofcode.com/2023/day/23
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-23-example.txt"
#define N 23
#else
#define NAME "../aocinput/2023-23-input.txt"
#define N 141
#endif
// #define QSIZE 1024
#define SSIZE 1024

typedef struct vec {
    int16_t x, y;
} Vec;

typedef enum dir {
    UP, DOWN, LEFT, RIGHT
} Dir;

typedef struct state {
    Vec pos;
    Dir dir;
    int len;
} State;

typedef struct node {
    Vec prev;
    int nbcount, pathlen;
    bool seen, isgoal;
} Node;

static const Vec delta[4] = {{0,-1},{0,1},{-1,0},{1,0}};  // up,down,left,right
static const char* dirchar = "UDLR";

static char maze[N][N + 2];  // +2='\n\0'
static Node node[N][N];

static State* stack;
static size_t ssize, slen;

// static State* queue;
// static size_t qsize, qlen, qhead, qtail;

static bool spop(State* const state)
{
    if (!stack || !slen) return false;  // empty stack
    *state = stack[--slen];
    return true;
}

static bool spush(const State state)
{
    if (!stack) {  // stack not allocated yet
        stack = malloc(SSIZE * sizeof *stack);
        if (!stack) { fputs("Could not allocate stack.", stderr); return false; }
        ssize = SSIZE;  // slen should still be zero
    } else if (slen == ssize) {  // stack is full
        size_t newsize = ssize << 1;  // ssize must be >0 already
        State* p = realloc(stack, newsize * sizeof *stack);
        if (!p) { fputs("Could not reallocate stack.", stderr); return false; }
        stack = p;
        ssize = newsize;
    }
    stack[slen++] = state;
    return true;
}

// static bool qpop(State* const s)
// {
//     if (!queue || qhead == qtail) return false;  // empty queue
//     *s = queue[qtail++];
//     if (qtail == qsize) qtail = 0;  // loop around
//     --qlen;
//     return true;
// }

// static bool qpush(const State s)
// {
//     if (!queue) {  // queue not allocated yet
//         queue = malloc(QSIZE * sizeof *queue);
//         if (!queue) { fputs("Could not allocate queue.", stderr); return false; }
//         qsize = QSIZE;  // qhead,qtail,qlen should all still be zero
//     } else if ((qhead + 1) % qsize == qtail) {  // queue is full
//         size_t newsize = qsize << 1;  // qsize must be >0 already
//         State* p = realloc(queue, newsize * sizeof *queue);
//         if (!p) { fputs("Could not reallocate queue.", stderr); return false; }
//         queue = p;
//         if (qtail) {
//             if (qhead < qtail) {  // head looped around, no room at the bottom
//                 memcpy(queue + qsize, queue, qhead * sizeof *queue);  // move out of the way
//                 qhead += qsize;
//             }
//             qhead -= qtail;  // now equal to qlen (if qlen was correct)
//             memmove(queue, queue + qtail, qhead * sizeof *queue);  // move to bottom
//             qtail = 0;
//         }
//         qsize = newsize;
//     }
//     queue[qhead++] = s;
//     if (qhead == qsize) qhead = 0;  // loop around
//     ++qlen;
//     return true;
// }

static bool isequal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static void explore(const Vec start, const Dir dir, const Vec goal)
{
    int maxlen = 0;
    State s = (State){start, 0};
    qpush(s);
    setseen(start);
    while (qpop(&s)) {
        if (!isequal(s.pos, goal)) {
        } else if (s.len > maxlen)
            maxlen = s.len;
    }
    return maxlen;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.", stderr); return 1; }
    for (int i = 0; i < N && fgets(maze[i], sizeof *maze, f); ++i);
    fclose(f);
    return 0;
}
