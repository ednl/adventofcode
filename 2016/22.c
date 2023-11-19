#include <stdio.h>
#include <stdlib.h>  // qsort, free
#include <string.h>  // memcpy

#define X 37
#define Y 25
#define N (X * Y)

typedef struct {
    int id, x, y, used, avail;
} node_t;

static node_t grid[N], copy1[N], copy2[N];

static int used_asc(const void *p, const void *q)
{
    const node_t *a = p;
    const node_t *b = q;
    if (a->used < b->used) return -1;
    if (a->used > b->used) return  1;
    return 0;
}

static int avail_desc(const void *p, const void *q)
{
    const node_t *a = p;
    const node_t *b = q;
    if (a->avail < b->avail) return  1;
    if (a->avail > b->avail) return -1;
    return 0;
}

static int parse(void)
{
    FILE *f = fopen("../aocinput/2016-22-input.txt", "r");
    if (!f)
        return 0;
    char *line = NULL;
    size_t size;
    ssize_t len;
    getline(&line, &size, f);
    getline(&line, &size, f);
    int n = 0, x, y, s, used, avail;
    while (n < N && (len = getline(&line, &size, f)) > 0)
        if (sscanf(line + 16, "%d-y%d %dT %dT %d", &x, &y, &s, &used, &avail) == 5) {
            grid[n] = (node_t){n, x, y, used, avail};
            ++n;
        }
    fclose(f);
    free(line);
    return n;
}

static int manh(int x0, int y0, int x1, int y1)
{
    return abs(x0 - x1) + abs(y0 - y1);
}

int main(void)
{
    int n = parse();
    printf("n=%d\n", n);
    memcpy(copy1, grid, sizeof grid);
    memcpy(copy2, grid, sizeof grid);
    qsort(copy1, (size_t)n, sizeof *copy1, used_asc);
    qsort(copy2, (size_t)n, sizeof *copy2, avail_desc);

    int viable = 0;
    node_t *a = copy1, *b = NULL, *empty = NULL;
    for (int i = 0; i < n; ++i) {
        b = copy2;
        if (a->used > b->avail)
            break;
        if (a->used)
            for (int j = 0; j < n; ++j) {
                if (a->used > b->avail)
                    break;
                viable += a->id != b->id;
                ++b;
            }
        else
            empty = &grid[a->id];
        ++a;
    }
    printf("Part 1: %d\n\n", viable);  // 892

    printf("  ");
    for (int x = 0; x < X; ++x)
        printf("%2d", x % 10);
    putchar('\n');

    for (int y = 0; y < Y; ++y) {
        printf("%2d", y);
        for (int x = 0; x < X; ++x) {
            putchar(' ');
            if (!x && !y) putchar('X');
            else if (x == X - 1 && !y) putchar('G');
            else if (x == empty->x && y == empty->y) putchar('E');
            else if (grid[x * Y + y].used > empty->avail) putchar('#');
            else putchar('.');
        }
        printf(" %d\n", y);
    }

    printf("  ");
    for (int x = 0; x < X; ++x)
        printf("%2d", x % 10);
    printf("\n\n");

    int moves = 0;
    printf("E  : (%2d,%2d)\n", empty->x, empty->y);
    printf("E  : (%2d,%2d) = %3d\n",  4, 7, (moves += manh(empty->x, empty->y, 4, 7)));
    printf("E  : (%2d,%2d) = %3d\n", 35, 0, (moves += manh(4, 7, 35, 0)));
    printf("EG : (%2d,%2d) = %3d\n",  0, 0, (moves += 5 * manh(35, 0, 0, 0)));
    printf("GE : (%2d,%2d) = %3d\n",  0, 0, ++moves);
    printf("\nPart 2: %d\n", moves);  // 227

    return 0;
}
