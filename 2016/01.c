#include <stdio.h>   // fopen, fscanf, fgetc, fclose, printf
#include <stdlib.h>  // abs
#include <stdbool.h>

typedef struct {
    int x, y;
} Vec;

static const Vec heading[4] = {{0,1}, {1,0}, {0,-1}, {-1,0}};  // N, E, S, W
static bool grid[500][500] = {0};

int main(void)
{
    FILE *f = fopen("../aocinput/2016-01-input.txt", "r");
    if (!f)
        return 1;

    size_t head = 0;  // initial heading = north
    Vec pos = {0};    // initial position = origin
    grid[250][250] = true;
    char turn;
    int dist, i = 0;
    bool part2 = true;
    while (fscanf(f, "%c%d", &turn, &dist) == 2) {
        ++i;
        ++head;
        if (turn == 'L')
            head += 2U;
        head &= 3U;
        if (part2) {
            for (int d = 0; d < dist; ++d) {
                pos.x += heading[head].x;
                pos.y += heading[head].y;
                if (part2) {
                    if (grid[pos.x + 250][pos.y + 250]) {
                        printf("Part 2: %d\n", abs(pos.x) + abs(pos.y));
                        part2 = false;
                    } else {
                        grid[pos.x + 250][pos.y + 250] = true;
                    }
                }
            }
        } else {
            pos.x += heading[head].x * dist;
            pos.y += heading[head].y * dist;
        }
        // printf("%3d: %c%3d => (%4d,%4d) = %4d\n", i, turn, dist, pos.x, pos.y, abs(pos.x) + abs(pos.y));
        fgetc(f);  // comma
        fgetc(f);  // space
    }
    fclose(f);
    printf("Part 1: %d\n", abs(pos.x) + abs(pos.y));
    return 0;
}
