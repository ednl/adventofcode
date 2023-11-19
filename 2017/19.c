#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM (201)  // width & height of the array in the puzzle file
#define LEN  (27)  // probably enough to hold all letters

static char diag[DIM][DIM + 1] = {0};  // diagram + \0 as EOL

static void load(void)
{
    FILE *f = fopen("../aocinput/2017-19-input.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t buflen = 0, line = 0;
    ssize_t len;
    while ((len = getline(&buf, &buflen, f)) != -1 && line < DIM) {
        memcpy(diag[line++], buf, len - 1);
    }
    fclose(f);
    free(buf);
}

int main(void)
{
    char part1[LEN] = {0};
    size_t len = 0;
    int part2 = 1;  // first pipe counts, too
    int i = 0, j = 0, dir;  // coordinates and step -1/+1

    load();
    // Find the start of the pipe on the top line
    while (diag[i][j] == ' ') {
        ++j;
    }

    while(1) {
        // Go up (-1) or go down (+1)
        dir = i == 0 || diag[i - 1][j] == ' ' ? 1 : -1;
        if (diag[i + dir][j] == ' ') {
            break;  // nowhere left to go
        }
        // Go vertically until the next char is a space
        while (diag[i + dir][j] != ' ') {
            i += dir;
            if (diag[i][j] >= 'A' && diag[i][j] <= 'Z') {
                part1[len++] = diag[i][j];  // save letter
            }
            ++part2;  // count steps
        }

        // Go left (-1) or go right (+1)
        dir = j == 0 || diag[i][j - 1] == ' ' ? 1 : -1;
        if (diag[i][j + dir] == ' ') {
            break;  // nowhere left to go
        }
        // Go horizontally until the next char is a space
        while (diag[i][j + dir] != ' ') {
            j += dir;
            if (diag[i][j] >= 'A' && diag[i][j] <= 'Z') {
                part1[len++] = diag[i][j];  // save letter
            }
            ++part2;  // count steps
        }
    }

    printf("Part 1: %s\n", part1);
    printf("Part 2: %d\n", part2);
    return 0;
}
