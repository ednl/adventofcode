#include <stdio.h>
#include <stdint.h>

// Puzzle parameters
#define GEOX 16807
#define GEOY 48271
#define EMOD 20183
#define TMOD     3

// Puzzle input
#define DEPTH  8787
#define TARGETX  10
#define TARGETY 725

typedef struct _region_t {
    uint64_t geo, ero, type;
} region_t;

static const char typestr[] = ".=|";
static region_t region[TARGETY + 1][TARGETX + 1];

int main(void)
{
    region[0][0] = region[TARGETY][TARGETX] = (region_t){0, DEPTH, DEPTH % TMOD};

    for (uint64_t x = 1; x <= TARGETX; ++x) {
        uint64_t g = x * GEOX;
        uint64_t e = (g + DEPTH) % EMOD;
        uint64_t t = e % TMOD;
        region[0][x] = (region_t){g, e, t};
    }

    for (uint64_t y = 1; y <= TARGETY; ++y) {
        uint64_t g = y * GEOY;
        uint64_t e = (g + DEPTH) % EMOD;
        uint64_t t = e % TMOD;
        region[y][0] = (region_t){g, e, t};
        for (uint64_t x = 1; x <= TARGETX; ++x) {
            g = region[y - 1][x].ero * region[y][x - 1].ero;
            e = (g + DEPTH) % EMOD;
            t = e % TMOD;
            region[y][x] = (region_t){g, e, t};
        }
    }

    uint64_t risk = 0;
    for (uint64_t y = 0; y <= TARGETY; ++y) {
        for (uint64_t x = 0; x <= TARGETX; ++x) {
            risk += region[y][x].type;
            if (!x && !y)
                putchar('M');
            else if (x == TARGETX && y == TARGETY)
                putchar('T');
            else
                printf("%c", typestr[region[y][x].type]);
        }
        putchar('\n');
    }
    printf("Part 1: %llu\n", risk);

    return 0;
}
