#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t pattern[4];
    uint8_t width, height;
} Shape;

static const Shape shape[] = {
    {{0x1e, 0x00, 0x00, 0x00}, 4, 1},  // -
    {{0x08, 0x1c, 0x08, 0x00}, 3, 3},  // +
    {{0x1c, 0x04, 0x04, 0x00}, 3, 3},  // L
    {{0x10, 0x10, 0x10, 0x10}, 1, 4},  // |
    {{0x18, 0x18, 0x00, 0x00}, 2, 2},  // o
};

static uint8_t *cave;
static size_t size, height;

static bool testdown(const Shape * const s, size_t y)
{
    if (!y--) return false;
    if (cave[y] & s->pattern[0]) return false;
    if (cave[y] & s->pattern[1]) return false;
    return true;
}

int main(void)
{
    for (size_t i = 0; i < sizeof shape / sizeof *shape; ++i) {
        for (uint8_t j = 0; j < shape[i].height; ++j) {
            int mask = 16;
            for (uint8_t k = 0; k < shape[i].width; ++k) {
                putchar(shape[i].pattern[j] & mask ? '#' : '.');
                mask >>= 1;
            }
            putchar('\n');
        }
        putchar('\n');
    }

    size = 256;
    cave = calloc(size, sizeof *cave);
    for (size_t i = 0; i < sizeof shape / sizeof *shape; ++i) {
        Shape s = shape[i];
        if (height + s.height > size) {
            size += 256;
            cave = realloc(cave, size * sizeof *cave);
            memset(cave + height, 0, size - height);
        }
        size_t x = 2, y = height;

    }
    free(cave);
    return 0;
}
