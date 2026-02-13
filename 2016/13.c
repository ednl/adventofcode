#include <stdio.h>
#include "../startstoptimer.h"

#define FAV 1364  // my puzzle input
#define TX    31  // target x
#define TY    39  // target y
#define QS   128  // queue size
#define SX     1  // start x
#define SY     1  // start y
#define W     33  // grid width  = TX + 2
#define H     41  // grid height = TY + 2
#define N     50  // max number of steps for part 2

typedef struct vec2 {
    int x, y;
} Vec2;

typedef struct step {
    Vec2 pos;
    int dist;
} Step;

typedef enum status {
    UNKNOWN, OPEN, EXPLORED, WALL
} Status;

static const Vec2 start = {SX, SY}, target = {TX, TY};
static const Vec2 dir[4] = {{1,0}, {0,1}, {-1,0}, {0,-1}};
static Status grid[H][W];
static Step queue[QS];

static Status status(const Vec2 p)
{
    if (p.x < 0 || p.y < 0)        return WALL;
    if (p.x >= W || p.y >= H)      return UNKNOWN;
    if (grid[p.y][p.x] != UNKNOWN) return grid[p.y][p.x];

    const int id = p.x * (p.x + 3) + p.y * (p.y + 1) + 2 * p.x * p.y + FAV;
    return (__builtin_popcount(id) & 1) ? (grid[p.y][p.x] = WALL) : (grid[p.y][p.x] = OPEN);
}

int main(void)
{
    starttimer();
    int tail = 0, head = 1, part1 = 0, part2 = 1;
    queue[0] = (Step){start, 0};
    grid[start.y][start.x] = EXPLORED;
    while (tail != head) {
        Step cur = queue[tail++];
        if (tail == QS)
            tail = 0;
        if (cur.pos.x == target.x && cur.pos.y == target.y)
            part1 = cur.dist;
        for (int i = 0; i < 4; ++i) {
            const Step next = {{cur.pos.x + dir[i].x, cur.pos.y + dir[i].y}, cur.dist + 1};
            if (status(next.pos) == OPEN) {
                queue[head++] = next;
                if (head == QS)
                    head = 0;
                grid[next.pos.y][next.pos.x] = EXPLORED;
                if (next.dist <= N)
                    ++part2;
            }
        }
    }
    printf("%d %d\n", part1, part2);  // 86 127
    printf("Time: %.0f us\n", stoptimer_us());
}
