#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-18-example.txt"
#define DIG 14
#define STACKSIZE 16
#else
#define NAME "../aocinput/2023-18-input.txt"
#define DIG 766
#define STACKSIZE 80000
#endif

typedef enum dir {
    NONE=0, UP, DOWN, LEFT, RIGHT
} Dir;

typedef struct vec {
    int x, y;
} Vec;

typedef struct dig {
    Dir dir;
    int len;
    unsigned rgb;
} Dig;

static const Vec delta[5] = {{0,0},{0,-1},{0,1},{-1,0},{1,0}};
static const char* dir2char = "_UDLR";
static Dig dig[DIG];
static char* dug;  // .=outside, #=trench, X=inside
static Vec stack[STACKSIZE];
static int stacklen;

static Dir char2dir(const char c)
{
    switch (c) {
        case 'U': return UP;
        case 'D': return DOWN;
        case 'L': return LEFT;
        case 'R': return RIGHT;
    }
    return NONE;
}

// static int min_i(const int a, const int b)
// {
//     return a < b ? a : b;
// }

// static int max_i(const int a, const int b)
// {
//     return a > b ? a : b;
// }

static void min_r(Vec* const min, const Vec a)
{
    if (a.x < min->x) min->x = a.x;
    if (a.y < min->y) min->y = a.y;
}

static void max_r(Vec* const max, const Vec a)
{
    if (a.x > max->x) max->x = a.x;
    if (a.y > max->y) max->y = a.y;
}

// static bool iszero(const Vec a)
// {
//     return !(a.x | a.y);
// }

static bool isequal(const Vec a, const Vec b)
{
    return !((a.x ^ b.x) | (a.y ^ b.y));
}

static Vec mul(const Vec a, const int mult)
{
    return (Vec){a.x * mult, a.y * mult};
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static void add_r(Vec* const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y};
}

// static void sub_r(Vec* const a, const Vec b)
// {
//     a->x -= b.x;
//     a->y -= b.y;
// }

static Vec go(const Vec pos, const Dir dir, const int len)
{
    return add(pos, mul(delta[dir], len));
}

static void go_r(Vec* pos, const Dir dir, const int len)
{
    add_r(pos, mul(delta[dir], len));
}

static char getdug(const Vec pos, const Vec dim)
{
    return dug[pos.y * dim.x + pos.x];
}

static void setdug(const Vec pos, const Vec dim, const char val)
{
    dug[pos.y * dim.x + pos.x] = val;
}

static void floodfill(Vec pos, const Vec dim, const char border, const char paint)
{
    int maxsl = 0;
    bool* done = calloc((size_t)(dim.x * dim.y), sizeof *done);
    stack[stacklen++] = pos;
    while (stacklen > 0 && stacklen < STACKSIZE - 4) {
        pos = stack[--stacklen];
        const int i = pos.y * dim.x + pos.x;
        dug[i] = paint;
        done[i] = true;
        if (pos.x > 0         && !done[i - 1]     && dug[i - 1]     != border) stack[stacklen++] = (Vec){pos.x - 1, pos.y};
        if (pos.x < dim.x - 1 && !done[i + 1]     && dug[i + 1]     != border) stack[stacklen++] = (Vec){pos.x + 1, pos.y};
        if (pos.y > 0         && !done[i - dim.x] && dug[i - dim.x] != border) stack[stacklen++] = (Vec){pos.x, pos.y - 1};
        if (pos.y < dim.y - 1 && !done[i + dim.x] && dug[i + dim.x] != border) stack[stacklen++] = (Vec){pos.x, pos.y + 1};
        if (stacklen > maxsl) maxsl = stacklen;
    }
    if (stacklen > 0)
        printf("!!!stacklen\n");
    printf("sl=%d\n", maxsl);
    free(done);
}

// static void floodfill(const Vec start, const Vec dim, const char border, const char paint)
// {
//     ffstack[ffstacklen++] = (FFstack){start.x, start.x, start.y, 1};
//     ffstack[ffstacklen++] = (FFstack){start.x, start.x, start.y - 1, -1};
//     while (ffstacklen) {
//         FFstack p = ffstack[--ffstacklen];
//         int x = p.x1;
//         if (getdug((Vec){x, p.y}, dim) != border) {
//             while (getdug((Vec){x - 1, p.y}, dim) != border)
//                 setdug((Vec){--x, p.y}, dim, paint);
//             if (x < p.x1)
//                 ffstack[ffstacklen++] = (FFstack){x, p.x1 - 1, p.y - p.dy, -p.dy};
//         }
//         while (p.x1 <= p.x2) {
//             while (getdug((Vec){p.x1, p.y}, dim) != border)
//                 setdug((Vec){p.x1++, p.y}, dim, paint);
//             if (p.x1 > x)
//                 ffstack[ffstacklen++] = (FFstack){x, p.x1 - 1, p.y + p.dy, p.dy};
//             if (p.x1 - 1 > p.x2)
//                 ffstack[ffstacklen++] = (FFstack){p.x2 + 1, p.x1 - 1, p.y - p.dy, -p.dy};
//             p.x1++;
//             while (p.x1 < p.x2 && getdug((Vec){p.x1, p.y}, dim) == border)
//                 p.x1++;
//             x = p.x1;
//         }
//     }
// }

static void show(const Vec dim)
{
#if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < dim.y; ++i) {
        for (int j = 0; j < dim.x; ++j)
            fputc(getdug((Vec){j, i}, dim), stdout);
        printf("\n");
    }
    printf("\n");
#endif
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    char c;
    unsigned len, rgb;
    for (int i = 0; i < DIG && fscanf(f, " %c %2u (#%6x)", &c, &len, &rgb) == 3; ++i)
        dig[i] = (Dig){char2dir(c), (int)len, rgb};
    fclose(f);

    #if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < DIG; ++i)
        printf("%c %2d (#%06x)\n", dir2char[dig[i].dir], dig[i].len, dig[i].rgb);
    printf("\n");
    #endif

    // Size up the lagoon
    Vec pos = {0}, min = {0}, max = {0};
    for (int i = 0; i < DIG; ++i) {
        go_r(&pos, dig[i].dir, dig[i].len);
        min_r(&min, pos);
        max_r(&max, pos);
        #if EXAMPLE || defined(DEBUG)
        printf("pos=(%3d,%3d) min=(%3d,%3d) max=(%3d,%3d)\n", pos.x, pos.y, min.x, min.y, max.x, max.y);
        #endif
    }
    printf("\n");

    // Set up the field
    max = add(sub(max, min), (Vec){4,4});  // border 2 on every side
    Vec dim = add(max, (Vec){1,1});  // len=diff+1
    pos = add(mul(min, -1), (Vec){2,2});  // starting position, +2 border
    min = (Vec){0,0};
    size_t count = (size_t)(dim.x * dim.y);
    dug = malloc(count);
    memset(dug, '?', count);
    memset(dug, '#', (size_t)dim.x);
    memset(dug + max.y * dim.x, '#', (size_t)dim.x);
    char* b = dug + max.x;  // end of first row
    for (int i = 1; i < dim.y; ++i) {
        *b = '#';
        *(b + 1) = '#';
        b += dim.x;
    }
    show(dim);
    printf("min=(%d,%d) max=(%d,%d) dim=(%d,%d) start=(%d,%d)\n\n", min.x, min.y, max.x, max.y, dim.x, dim.y, pos.x, pos.y);
    // return 0;

    // Dig the trench
    for (int i = 0; i < DIG; ++i) {
        const Vec next = go(pos, dig[i].dir, dig[i].len);
        const Vec step = delta[dig[i].dir];
        while (!isequal(pos, next)) {
            add_r(&pos, step);
            setdug(pos, dim, '#');
        }
    }
    show(dim);

    // Fill OUTSIDE the lagoon
    floodfill((Vec){1,1}, dim, '#', '.');
    memset(dug, '.', (size_t)dim.x);
    memset(dug + max.y * dim.x, '.', (size_t)dim.x);
    b = dug + max.x;  // end of first row
    for (int i = 1; i < dim.y; ++i) {
        *b = '.';
        *(b + 1) = '.';
        b += dim.x;
    }
    show(dim);

    // Fill inside the lagoon
    for (int i = 0; i < dim.y; ++i)
        for (int j = 0; j < dim.x; ++j)
            if (getdug((Vec){j,i}, dim) == '?')
                setdug((Vec){j,i}, dim, 'X');
    show(dim);

    int part1 = 0;
    for (int i = 0; i < dim.y; ++i)
        for (int j = 0; j < dim.x; ++j)
            part1 += getdug((Vec){j,i}, dim) != '.';
    printf("Part 1: %d\n", part1);  // example: 62, input: 46334

    free(dug);
    return 0;
}
