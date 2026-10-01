#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SIZE (1 << 12)
#define MASK (SIZE - 1)

typedef struct map {
    uint32_t index, key;
} Map;

static Map map[SIZE];

static uint32_t fnv1a(uint32_t x)
{
    uint32_t h = 0x811c9dc5;
    for (; x; x >>= 8) {
        h ^= x & 255;
        h *= 0x01000193;
    }
    return h;
}

static int cmp(const void *p, const void *q)
{
    const Map *a = p, *b = q;
    if (a->index < b->index) return -1;
    if (a->index > b->index) return +1;
    if (a->key < b->key) return -1;
    if (a->key > b->key) return +1;
    return 0;
}

int main(void)
{
    int n = 0;
    FILE *f = fopen("../aocinput/2024-01-input.txt", "r");
    for (int a, b; fscanf(f, "%d %d", &a, &b) == 2; ) {
        map[n++] = (Map){fnv1a(a) & MASK, a};
        map[n++] = (Map){fnv1a(b) & MASK, b};
    }
    fclose(f);
    qsort(map, n, sizeof *map, cmp);
    int m = 0;
    for (int i = 1; i < n; ++i)
        if (map[i - 1].index == map[i].index && map[i - 1].key != map[i].key)
            printf("%3u: %4u -> %5u %5u\n", ++m, map[i].index, map[i - 1].key, map[i].key);
}
