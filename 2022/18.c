/**
 * Advent of Code 2022
 * Day 18: Boiling Boulders
 * https://adventofcode.com/2022/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

/* Example layout:
   0   1   2   3   4   5
  012 012 012 012 012 012
0 ... .#. ... ... .#. ...
1 .#. ### .#. .#. #.# .#.
2 ... .#. ... ... .#. ...

*/

#include <stdio.h>  // printf, fscanf
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "../aocinput/2022-18-example.txt"
#define N 25
#else
#define NAME "../aocinput/2022-18-input.txt"
#define N 2500
#endif

#define L 20        // coordinate range from input file (0..19)
#define S 2500  // stack size from trial & error: 2000 = too small

typedef struct {
    int x, y, z;
} Pos;
typedef enum {AIR,LAVA,STEAM} Scan;

static size_t count;        // number of lines in input file, must be <= N
static Pos lava[N];         // store data from input file
static Scan scan[L][L][L];  // lava droplet geometry on a grid
static Pos stack[S];        // stack of positions (x,y,z) for floodfill

static size_t readinput(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 0;  // file not found = no data
    size_t n = 0;
    int x, y, z;
    while (n < N && fscanf(f, "%d,%d,%d", &x, &y, &z) == 3)
        if (x >= 0 && y >= 0 && z >= 0 && x < L && y < L && z < L) {
            lava[n++] = (Pos){x,y,z};
            scan[x][y][z] = LAVA;
        }
    fclose(f);
    return n;
}

static int surfacearea(const Scan detect)
{
    int exposed = 0;
    for (size_t i = 0; i < count; ++i) {
        Pos *p = &lava[i];
        if (p->x == 0 || scan[p->x - 1][p->y][p->z] == detect)
            exposed++;
        if (p->y == 0 || scan[p->x][p->y - 1][p->z] == detect)
            exposed++;
        if (p->z == 0 || scan[p->x][p->y][p->z - 1] == detect)
            exposed++;
        if (p->x == L - 1 || scan[p->x + 1][p->y][p->z] == detect)
            exposed++;
        if (p->y == L - 1 || scan[p->x][p->y + 1][p->z] == detect)
            exposed++;
        if (p->z == L - 1 || scan[p->x][p->y][p->z + 1] == detect)
            exposed++;
    }
    return exposed;
}

static void floodfill(Pos p)
{
    size_t sp = 0;
    scan[p.x][p.y][p.z] = STEAM;
    stack[sp++] = p;
    while (sp) {
        p = stack[--sp];
        // if (sp + 6 > SSIZE) {
        //     fprintf(stderr, "Stack overflow\n");
        //     exit(1);
        // }
        if (p.x > 0 && scan[p.x - 1][p.y][p.z] == AIR) {
            scan[p.x - 1][p.y][p.z] = STEAM;
            stack[sp++] = (Pos){p.x - 1, p.y, p.z};
        }
        if (p.y > 0 && scan[p.x][p.y - 1][p.z] == AIR) {
            scan[p.x][p.y - 1][p.z] = STEAM;
            stack[sp++] = (Pos){p.x, p.y - 1, p.z};
        }
        if (p.z > 0 && scan[p.x][p.y][p.z - 1] == AIR) {
            scan[p.x][p.y][p.z - 1] = STEAM;
            stack[sp++] = (Pos){p.x, p.y, p.z - 1};
        }
        if (p.x < L - 1 && scan[p.x + 1][p.y][p.z] == AIR) {
            scan[p.x + 1][p.y][p.z] = STEAM;
            stack[sp++] = (Pos){p.x + 1, p.y, p.z};
        }
        if (p.y < L - 1 && scan[p.x][p.y + 1][p.z] == AIR) {
            scan[p.x][p.y + 1][p.z] = STEAM;
            stack[sp++] = (Pos){p.x, p.y + 1, p.z};
        }
        if (p.z < L - 1 && scan[p.x][p.y][p.z + 1] == AIR) {
            scan[p.x][p.y][p.z + 1] = STEAM;
            stack[sp++] = (Pos){p.x, p.y, p.z + 1};
        }
    }
}

int main(void)
{
    starttimer();

    count = readinput();
    if (!count)
        return 1;

    printf("Part 1: %d\n", surfacearea(AIR));  // example=64, input=3346

    floodfill((Pos){0,0,0});  // far enough away from droplet to catch all entrances
    printf("Part 2: %d\n", surfacearea(STEAM));  // example=58, input=1980

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
