#include <stdio.h>

#define N 3014387

int main(int argc, char *argv[])
{
    int n = N;
    if (argc == 2)
        sscanf(argv[1], "%d", &n);

    // // My own solution after thinking hard
    // int len = n, winner = 1, step = 2;
    // while (len > 1) {
    //     if (len & 1)
    //         winner += step;
    //     len >>= 1;
    //     step <<= 1;
    // }
    // printf("Part 1: %d\n", winner);  // 1834471

    // Or solution to the Josephus Problem
    // from https://www.youtube.com/watch?v=uCsD3ZGzMgE
    // int nn = n, msb = 1;
    // while (nn >>= 1)
    //     msb <<= 1;
    // int winner = ((n ^ msb) << 1) | 1;
    printf("Part 1: %d\n", ((N ^ ((1u << 31) >> __builtin_clz(N))) << 1) | 1);  // 1834471

    // 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
    // 1 3 1 2 3 5 7 9  1  2  3  4  5  6  7  8  9 11 13 15 17 19 21 23 25 27  1  2  3
    int p = 1;
    while (p * 3 < n)
        p *= 3;
    int winner = n - p;
    if (n > p * 2)
        winner += n - p * 2;
    printf("Part 2: %d\n", winner);  // 1420064
    return 0;
}
