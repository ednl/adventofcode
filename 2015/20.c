#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define N  (1000 * 1000)  // number of houses (arbitrary)
#define M1 10  // elf multiplier (part 1)
#define M2 11  // elf multiplier (part 2)
#define Q  50  // max house visits (part 2)

static int presents[N];

int main(void)
{
    FILE *f = fopen("../aocinput/2015-20-input.txt", "r");
    if (!f) return 1;
    int P;  // puzzle input, presents threshold
    if (fscanf(f, "%d", &P) != 1) return 2;
    if (P < 1 || P > (1u << 31) - 1) return 3;

#ifdef TIMER
    starttimer();
#endif

    // Part 1: distribute presents
    for (int house = 1; house < N; ++house)
        presents[house] = M1;
    for (int elf = 2; elf < N; ++elf) {
        const int npres = elf * M1;
        for (int house = elf; house < N; house += elf)
            presents[house] += npres;
    }

    // Part 1: find lowest house number where number of presents >= puzzle input
    for (int house = 1; house < N; ++house)
        if (presents[house] >= P) {
            printf("Part 1: %d (%d >= %d)\n", house, presents[house], P);  // 776160
            break;
        }

    // Part 2: distribute presents
    for (int house = 1; house < N; ++house)
        presents[house] = M2;
    for (int elf = 2; elf < N; ++elf) {
        const int npres = elf * M2;
        for (int visit = 0, house = elf; visit < Q && house < N; ++visit, house += elf)
            presents[house] += npres;
    }

    // Part 2: find lowest house number where number of presents >= puzzle input
    for (int house = 1; house < N; ++house)
        if (presents[house] >= P) {
            printf("Part 2: %d (%d >= %d)\n", house, presents[house], P);  // 786240
            break;
        }

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
