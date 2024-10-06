#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define N 13
#define HSIZE 128

typedef struct vec {
    int x, y;
} Vec;

typedef struct cart {
    Vec pos;
    int dir;
} Cart;

typedef struct heap {
    int key;
    Cart state;
} Heap;

static const Vec starting = {0, 0}, destination = {N - 1, N - 1};
static char map[N][N + 2];
static bool seen[N][N][4];  // 4 directions
static int dist[N][N];  // 0=infinity
static Heap *heap;  // min heap
static int hlen, hsize;

static int zip(const Cart state)
{
    return (state.dir << 16) | (state.pos.x << 8) | state.pos.y;
}

static Cart unzip(const int state)
{
    return (Cart){{state >> 8 & 0xff, state & 0xff}, state >> 16 & 0xff};
}

static void swap(const int i, const int j)
{
    const Heap tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
}

static bool push(const Heap entry)
{
    if (hlen == HSIZE) {
        int newsize = hsize ? hsize << 1 : HSIZE;
        Heap *p = realloc(heap, (size_t)newsize);
        if (!p) { printf("!!! hsize=%d\n", hsize); return false; }
        heap = p;
        hsize = newsize;
    }
    heap[hlen++] = entry;
    for (int i = hlen - 1, parent; i > 0; i = parent) {
        parent = (i - 1) >> 1;
        if (heap[parent].key <= heap[i].key)
            return true;  // correct order for min heap
        swap(i, parent);
    }
    return true;
}

static bool pop(Heap *entry)
{
    if (!hlen) return false;
    *entry = heap[0];
    if (!--hlen) return true;
    heap[0] = heap[hlen];
    if (hlen == 1) return true;
    for (int i = 0, smallest = 0; ; i = smallest) {
        int child1 = (i << 1) + 1, child2 = child1 + 1;
        if (child1 < hlen && heap[child1].key < heap[smallest].key) smallest = child1;
        if (child2 < hlen && heap[child2].key < heap[smallest].key) smallest = child2;
        if (smallest == i)
            return true;  // correct order for min heap
        swap(i, smallest);
    }
}

static inline int absi(const int x)
{
    return x >= 0 ? x : -x;
}

static inline int sign(const int x)
{
    return (x > 0) - (x < 0);
}

static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static int norm1(const Vec a)
{
    return absi(a.x) + absi(a.y);
}

static int manh(const Vec a, const Vec b)
{
    return absi(b.x - a.x) + absi(b.y - a.y);
}

static Vec direction(const Vec a, const Vec b)
{
    return (Vec){sign(b.x - a.x), sign(b.y - a.y)};
}

static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static void mul_r(Vec *const a, const int mult)
{
    a->x *= mult;
    a->y *= mult;
}

static int heatloss(const Vec pos)
{
    return '0' | map[pos.y][pos.x];
}

static int cost(Vec a, const Vec b)
{
    const Vec step = direction(a, b);
    if (norm1(step) != 1)  // exactly one element must be 0
        return -1;
    int sum = 0;
    while (!equal(a, b)) {
        add_r(&a, step);
        sum += heatloss(a);
    }
    return sum;
}

static int dijkstra(Cart start)
{
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2023-17-example.txt", "r");
    for (int i = 0; i < N; ++i)
        fgets(map[i], sizeof *map, f);
    fclose(f);

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            map[i][j] &= 15;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            fputc(heatloss((Vec){j, i}), stdout);
        printf("\n");
    }

    return 0;
}
