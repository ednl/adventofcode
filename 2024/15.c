#include <stdio.h>

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-15-example1.txt"
    #define N 8
    #define L 1
    #define M 15
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-15-example2.txt"
    #define N 10
    #define L 10
    #define M 70
#else
    #define FNAME "../aocinput/2024-15-input.txt"
    #define N 50
    #define L 20
    #define M 1000
#endif
#define MOVES (L * M)
#define FLOOR '.'
#define WALL  '#'
#define BOX   'O'
#define ROBOT '@'

typedef struct pos {
    int x, y;
} Pos;

typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

static const Pos step[] = {{1,0},{0,1},{-1,0},{0,-1}};
static const Pos back[] = {{-1,0},{0,-1},{1,0},{0,1}};

static char map[N][N + 1];
static char move[L][M + 1];

static void show(void)
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            putchar(map[i][j]);
        putchar('\n');
    }
}

static inline Pos add(const Pos a, const Pos b)
{
    return (Pos){a.x + b.x, a.y + b.y};
}

static inline void add_r(Pos *const a, const Pos b)
{
    a->x += b.x;
    a->y += b.y;
}

static inline char peek(const Pos pos)
{
    return map[pos.y][pos.x];
}

static inline void poke(const Pos pos, const char tile)
{
    map[pos.y][pos.x] = tile;
}

static Pos findrobot(void)
{
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == ROBOT)
                return (Pos){j, i};
    return (Pos){0};
}

static void shift(Pos *const robot, const Dir dir, const int amount)
{
    Pos end = add(*robot, step[dir]);
    int floor = 0, len;
    for (len = 0; peek(end) != WALL && floor < amount; ++len) {
        floor += peek(end) == FLOOR;
        add_r(&end, step[dir]);
    }
    if (!floor)
        return;
    int boxes = len - floor;
    while (boxes--) {
        add_r(&end, back[dir]);
        poke(end, BOX);
    }
    add_r(&end, back[dir]);
    poke(end, ROBOT);
    *robot = end;
    while (floor--) {
        add_r(&end, back[dir]);
        poke(end, FLOOR);
    }
}

static void combomove(Pos *const robot, const char instr, const int amount)
{
    switch (instr) {
        case '<': shift(robot, LEFT , amount); break;
        case '>': shift(robot, RIGHT, amount); break;
        case '^': shift(robot, UP   , amount); break;
        case 'v': shift(robot, DOWN , amount); break;
    }
}

static int gps(void)
{
    int sum = 0;
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == BOX)
                sum += i * 100 + j;
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);
    fgetc(f);  // empty line
    fread(move, sizeof move, 1, f);

    Pos robot = findrobot();
    char prev = move[0][0];
    int amount = 0;
    for (int i = 0; i < L; ++i)
        for (int j = 0; j < M; ++j) {
            if (move[i][j] != prev) {
                combomove(&robot, prev, amount);
                prev = move[i][j];
                amount = 1;
            } else
                ++amount;
        }
    combomove(&robot, prev, amount);  // last batch
    show();
    printf("Part 1: %d\n", gps());  // example 1: 2028, example 2: 10092, input: 1497888
    return 0;
}
