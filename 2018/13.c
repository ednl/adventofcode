#include <stdio.h>    // fopen, fgetc, fclose, printf, putchar, fputs
#include <stdlib.h>   // qsort
#include <stdbool.h>  // bool

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "../aocinput/2018-13-example.txt"
#define GRIDCOLS 13
#define GRIDROWS  6
#define CARTLEN   2
#elif EXAMPLE == 2
#define NAME "../aocinput/2018-13-example2.txt"
#define GRIDCOLS  7
#define GRIDROWS  7
#define CARTLEN   9
#else
#define NAME "../aocinput/2018-13-input.txt"
#define GRIDCOLS 150
#define GRIDROWS 150
#define CARTLEN   17
#endif

// Tile form: ' '=0, |=1, -=2, /=3, '\'=4, +0=5, +1=6, +2=7, len=8
typedef enum _tile_t {
    EMPTY, VERT, HORZ, FWD, BACK, INTER, INTR1, INTR2, TILELEN
} tile_t;

// Cart heading: X=0, ^=1, >=2, v=3, <=4, len=5
typedef enum _head_t {
    NONE, UP, RIGHT, DOWN, LEFT, HEADLEN
} head_t;

typedef struct _pos_t {
    int x, y;
} pos_t;

typedef struct _cart_t {
    head_t head;
    pos_t pos;
    int turns;
} cart_t;

static const head_t turn[HEADLEN][TILELEN] = {
    {NONE , NONE , NONE , NONE , NONE , NONE , NONE , NONE },  // 'X :X', 'X|:X', 'X-:X', 'X/:X', 'X\:X', 'X+0:X', 'X+1:X', 'X+2:X'
    {NONE , UP   , NONE , RIGHT, LEFT , LEFT , UP   , RIGHT},  // '^ :X', '^|:^', '^-:X', '^/:>', '^\:<', '^+0:<', '^+1:^', '^+2:>'
    {NONE , NONE , RIGHT, UP   , DOWN , UP   , RIGHT, DOWN },  // '> :X', '>|:X', '>-:>', '>/:^', '>\:v', '>+0:^', '>+1:>', '>+2:v'
    {NONE , DOWN , NONE , LEFT , RIGHT, RIGHT, DOWN , LEFT },  // 'v :X', 'v|:v', 'v-:X', 'v/:<', 'v\:>', 'v+0:>', 'v+1:v', 'v+2:<'
    {NONE , NONE , LEFT , DOWN , UP   , DOWN , LEFT , UP   }   // '< :X', '<|:X', '<-:<', '</:v', '<\:^', '<+0:v', '<+1:<', '<+2:^'
};

static const pos_t step[HEADLEN] = {
    { 0,  0},  // X
    { 0, -1},  // ^
    { 1,  0},  // >
    { 0,  1},  // v
    {-1,  0}   // <
};

static tile_t grid[GRIDROWS][GRIDCOLS];
static cart_t cart[CARTLEN];
static int cols, rows, carts;

static void show(void)
{
#if EXAMPLE
    static const char tile[] = " |-/\\+++?";
    static const char head[] = "X^>v<?";

    int i = 0;
    for (int y = 0; y < rows; ++y) {
        printf("%3d: ", y);
        for (int x = 0; x < cols; ++x)
            if (i < carts && cart[i].pos.y == y && cart[i].pos.x == x)
                putchar(head[cart[i++].head]);
            else
                putchar(tile[grid[y][x]]);
        putchar('\n');
    }
#endif
}

static int cmp_carts(const void * a, const void * b)
{
    const cart_t *p = a, *q = b;
    if (p->head != NONE && q->head == NONE) return -1;
    if (p->head == NONE && q->head != NONE) return  1;
    if (p->pos.y < q->pos.y) return -1;
    if (p->pos.y > q->pos.y) return  1;
    if (p->pos.x < q->pos.x) return -1;
    if (p->pos.x > q->pos.x) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f) {
        fputs("File not found.", stderr);
        return 1;
    }
    int c, x = 0;
    while (rows < GRIDROWS && (c = fgetc(f)) != EOF) {
        if (c == '\n') {
            x = 0;
            ++rows;
        } else if (x < GRIDCOLS) {
            head_t h = NONE;
            switch (c) {
                case '|' : grid[rows][x] = VERT;  break;
                case '-' : grid[rows][x] = HORZ;  break;
                case '/' : grid[rows][x] = FWD;   break;
                case '\\': grid[rows][x] = BACK;  break;
                case '+' : grid[rows][x] = INTER; break;
                case '^' : grid[rows][x] = VERT;  h = UP;    break;
                case '>' : grid[rows][x] = HORZ;  h = RIGHT; break;
                case 'v' : grid[rows][x] = VERT;  h = DOWN;  break;
                case '<' : grid[rows][x] = HORZ;  h = LEFT;  break;
            }
            if (h != NONE && carts < CARTLEN)
                cart[carts++] = (cart_t){h, (pos_t){x, rows}, 0};
            if (++x > cols)
                cols = x;
        }
    }
    fclose(f);
    if (x != 0 && rows < GRIDROWS)
        ++rows;
    show();

    bool firstcrash = true;
    while (carts > 1) {
        for (int i = 0; i < carts; ++i) {
            cart[i].pos.x += step[cart[i].head].x;
            cart[i].pos.y += step[cart[i].head].y;
            for (int j = 0; j < carts; ++j)
                if (i != j && cart[i].pos.x == cart[j].pos.x && cart[i].pos.y == cart[j].pos.y) {
                    cart[i].head = cart[j].head = NONE;  // crash = remove from list
                    if (firstcrash) {
                        printf("Part 1: %d,%d\n", cart[i].pos.x, cart[i].pos.y);  // example1=(7,3) example2=(2,0) part1=(41,17)
                        firstcrash = false;
                    }
                    break;
                }
            tile_t t = grid[cart[i].pos.y][cart[i].pos.x];
            if (t == INTER)
                t += cart[i].turns++ % 3;
            cart[i].head = turn[cart[i].head][t];
        }
        qsort(cart, (size_t)carts, sizeof *cart, cmp_carts);
        while (carts > 0 && cart[carts - 1].head == NONE)
            --carts;
        show();
    }
    printf("Part 2: %d,%d\n", cart[0].pos.x, cart[0].pos.y);  // example1=(7,3) example2=(6,4) part2=(134,117)
    return 0;
}
