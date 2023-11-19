#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITSIZE  (25U)
#define PAD       (50U)
#define CLEAN     (0)
#define WEAKENED  (1)
#define INFECTED  (2)
#define FLAGGED   (3)
#define MASK      (3)

static bool addrow_below(char **g, unsigned int *r, const unsigned int c)
{
    char *t = realloc(*g, (*r + PAD) * c);
    if (t == NULL) {
        return false;
    }
    memset(t + *r * c, CLEAN, c * PAD);  // clear new last rows (use old row count)
    *g = t;
    *r += PAD;
    return true;
}

static bool addrow_above(char **g, unsigned int *r, const unsigned int c)
{
    char *t = realloc(*g, (*r + PAD) * c);
    if (t == NULL) {
        return false;
    }
    memmove(t + c * PAD, t, *r * c);  // move grid down (use old row count)
    memset(t, CLEAN, c * PAD);        // clear new first row
    *g = t;
    *r += PAD;
    return true;
}

static bool addcol(char **g, const unsigned int r, unsigned int *c, const unsigned int shift)
{
    const unsigned c1 = *c + PAD;
    char *t = malloc(r * c1);
    if (t == NULL) {
        return false;
    }
    memset(t, CLEAN, r * c1);
    for (unsigned int i = 0; i < r; ++i) {
        memcpy(t + i * c1 + shift, *g + i * *c, *c);
    }
    free(*g);
    *g = t;
    *c = c1;
    return true;
}

static bool addcol_right(char **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, 0);
}

static bool addcol_left(char **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, PAD);
}

static void print(const char *g, const unsigned int r, const unsigned int c)
{
    char ch = '?';
    for (unsigned int i = 0; i < r; ++i) {
        for (unsigned int j = 0; j < c; ++j) {
            switch (g[i * c + j]) {
                case CLEAN   : ch = '.'; break;
                case WEAKENED: ch = 'W'; break;
                case INFECTED: ch = '#'; break;
                case FLAGGED : ch = 'F'; break;
                default:       ch = '?'; break;
            }
            printf("%c", ch);
        }
        printf("\n");
    }
}

static void init(char **g, unsigned int *r, unsigned int *c)
{
    if (*g != NULL) {
        free(*g);
    }
    *g = calloc(INITSIZE * INITSIZE, sizeof(char));
    if (*g == NULL) {
        fprintf(stderr, "Out of memory");
        exit(2);
    }
    *r = *c = INITSIZE;

    FILE *f = fopen("../aocinput/2017-22-input.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t bufsize = 0;
    ssize_t len;
    unsigned int i = 0, j;
    while ((len = getline(&buf, &bufsize, f)) > 0 && i < INITSIZE) {
        char *pc = buf;
        j = 0;
        while (*pc && j < INITSIZE) {
            (*g)[i * INITSIZE + j++] = *pc++ == '#' ? INFECTED : CLEAN;
        }
        ++i;
    }
    free(buf);
    fclose(f);
}

static unsigned int evolve(char **g, unsigned int *r, unsigned int *c, const char part)
{
    const char step = 3 - part;  // next state: part 1 = 0/2, part 2 = 0/1/2/3
    const int bursts = part == 1 ? 10000 : 10000000;

    unsigned int infected = 0;
    unsigned int i = *r / 2, j = *c / 2;  // start in the middle
    char face = 0;  // face 0=up, 1=right, 2=down, 3=left

    for (int burst = 0; burst < bursts; ++burst) {
        if (i == 0) {
            addrow_above(g, r, *c);
            i = PAD;
        } else if (i == *r - 1) {
            addrow_below(g, r, *c);
        }
        if (j == 0) {
            addcol_left(g, *r, c);
            j = PAD;
        } else if (j == *c - 1) {
            addcol_right(g, *r, c);
        }
        unsigned int index = i * *c + j;
        switch ((*g)[index]) {
            case CLEAN:
                face = (face + 3) & MASK;  // turn left
                break;
            case INFECTED:
                face = (face + 1) & MASK;  // turn right
                break;
            case FLAGGED:
                face = (face + 2) & MASK;  // reverse
                break;
        }
        (*g)[index] = ((*g)[index] + step) & MASK;  // next state
        infected += (*g)[index] == INFECTED;
        switch(face) {
            case 0: --i; break;
            case 1: ++j; break;
            case 2: ++i; break;
            case 3: --j; break;
        }
    }
    return infected;
}

int main(void)
{
    char *grid = NULL;
    unsigned int rows, cols;

    // Part 1
    init(&grid, &rows, &cols);
    printf("Part 1: %u\n", evolve(&grid, &rows, &cols, 1)); // right answer = 5261
    // printf("%u %u\n", rows, cols);  // 55 75 for PAD=10
    // print(grid, rows, cols);  // debug

    // Part 2
    init(&grid, &rows, &cols);
    printf("Part 2: %u\n", evolve(&grid, &rows, &cols, 2)); // right answer = 2511927
    // printf("%u %u\n", rows, cols);  // 415 385 for PAD=10

    free(grid);
    return 0;
}
