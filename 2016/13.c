#include <stdio.h>
#include <stdbool.h>

#define EXAMPLE 0
#if EXAMPLE == 1
#define FAV    10  // example input
#define TX      7  // target x
#define TY      4  // target y
#define QS     10  // queue size
#else
#define FAV  1364  // my puzzle input
#define TX     31  // target x
#define TY     39  // target y
#define QS    100  // queue size
#endif
#define SX      1  // start x
#define SY      1  // start y
#define W      33  // grid width  = TX(input) + 2
#define H      41  // grid height = TY(input) + 2
#define N      50  // max number of steps for part 2

typedef struct {
    int x, y;
} vec_t;

typedef struct {
    vec_t pos;
    int dist;
} step_t;

typedef enum {
    UNKNOWN, OPEN, EXPLORED, WALL
} status_t;

static const vec_t start = {SX, SY}, target = {TX, TY};
static const vec_t dir[4] = {{1,0}, {0,1}, {-1,0}, {0,-1}};
static status_t grid[H][W];
static step_t queue[QS];

static status_t status(const vec_t p)
{
    if (p.x < 0 || p.y < 0)        return WALL;
    if (p.x >= W || p.y >= H)      return UNKNOWN;
    if (grid[p.y][p.x] != UNKNOWN) return grid[p.y][p.x];

    bool odd = false;
    int id = p.x * p.x + 3 * p.x + 2 * p.x * p.y + p.y + p.y * p.y + FAV;
    while (id) {
        if (id & 1)
            odd = !odd;
        id >>= 1;
    }
    return odd ? (grid[p.y][p.x] = WALL) : (grid[p.y][p.x] = OPEN);
}

int main(void)
{
    int pop = 0, ins = 1, part1 = 0, part2 = 1;
    queue[0] = (step_t){start, 0};
    grid[start.y][start.x] = EXPLORED;

    while (pop != ins) {
        step_t cur = queue[pop++];
        if (pop == QS)
            pop = 0;
        if (cur.pos.x == target.x && cur.pos.y == target.y)
            part1 = cur.dist;
        for (int i = 0; i < 4; ++i) {
            const step_t next = {{cur.pos.x + dir[i].x, cur.pos.y + dir[i].y}, cur.dist + 1};
            if (status(next.pos) == OPEN) {
                queue[ins++] = next;
                if (ins == QS)
                    ins = 0;
                grid[next.pos.y][next.pos.x] = EXPLORED;
                if (next.dist <= N)
                    ++part2;
            }
        }
    }
    printf("Part 1: %d\n", part1);  // example=11, input=86
    printf("Part 2: %d\n", part2);  // example=107, input=127
    return 0;
}
