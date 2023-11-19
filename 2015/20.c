#include <stdio.h>

#define P  33100000U  // puzzle input
#define N   1000000U  // number of houses (arbitrary)
#define M1       10U  // elf multiplier (part 1)
#define M2       11U  // elf multiplier (part 2)
#define Q        50U  // max house visits (part 2)

unsigned int presents[N] = {M1};  // init for part 1

int main(void)
{
    unsigned int elf, house, npres, visit;

    // Part 1: distribute presents (elf 1 already visited by init)
    for (elf = 2; elf < N; ++elf) {
        npres = elf * M1;
        for (house = elf; house < N; house += elf) {
            presents[house] += npres;
        }
    }

    // Part 1: find lowest house number where number of presents >= puzzle input
    for (house = 1; house < N; ++house) {
        if (presents[house] >= P) {
            printf("Part 1: %u\n", house);
            break;
        }
    }

    // Part 2: distribute presents
    for (house = 1; house < N; ++house) {
        presents[house] = M2;
    }
    for (elf = 2; elf < N; ++elf) {
        npres = elf * M2;
        visit = 0;
        for (house = elf; visit++ < Q && house < N; house += elf) {
            presents[house] += npres;
        }
    }

    // Part 2: find lowest house number where number of presents >= puzzle input
    for (house = 1; house < N; ++house) {
        if (presents[house] >= P) {
            printf("Part 2: %u\n", house);
            break;
        }
    }

    return 0;
}
