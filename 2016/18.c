/**
 * Advent of Code 2016
 * Day 18: Like a Rogue
 * https://adventofcode.com/2016/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark on a Mac Mini M1, compiler Apple clang 14.0.3 with -Ofast -march=native:
 *
 *     $ hyperfine -N -w 500 -r 1000 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):       1.6 ms ±   0.1 ms    [User: 1.0 ms, System: 0.4 ms]
 *       Range (min … max):     1.5 ms …   2.2 ms    1000 runs
 *
 * Benchmark on a Raspberry Pi 4, compiler Debian gcc 10.2.1-6 with -Ofast -march=native
 * and the CPU in performance mode:
 *
 *     echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *     (to reset, replace performance with ondemand)
 *     /boot/config.txt: arm_boost=1, no overclock
 *
 *     $ hyperfine -N -w 100 -r 500 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):       4.9 ms ±   0.1 ms    [User: 3.6 ms, System: 1.0 ms]
 *       Range (min … max):     4.7 ms …   5.5 ms    500 runs
 *
 * The rules in the puzzle description say:
 *
 *     Then, a new tile is a trap only in one of the following situations:
 *     - Its left and center tiles are traps, but its right tile is not.
 *     - Its center and right tiles are traps, but its left tile is not.
 *     - Only its left tile is a trap.
 *     - Only its right tile is a trap.
 *     In any other situation, the new tile is safe.
 *
 * Rules 3 and 4 imply that the centre tile as mentioned in rules 1 and 2
 * doesn't matter, so rules 1 and 2 as a whole are redundant. Rules 3 and 4
 * combined say: if left is different from right, the new tile is a trap.
 *
 * To test difference, use XOR (or in C, ^. Hey! That trap was probably a clue!).
 *
 *     0 XOR 0 = 0 (left=safe, right=safe => new tile=safe)
 *     0 XOR 1 = 1 (left=safe, right=trap => new tile=trap)
 *     1 XOR 0 = 1 (left=trap, right=safe => new tile=trap)
 *     1 XOR 1 = 0 (left=trap, right=trap => new tile=safe)
 *
 * To make bits go left or right, use shift (SHL and SHR, or in C, << and >>).
 * Added bits at the low or high end will be 0. For 4-bit numbers:
 *
 *     1011 SHL 1 = 0110
 *     0110 SHR 1 = 0011
 *
 * So a floor with two safe tiles and two traps (..^^) evolves as follows:
 *
 *     ..^^ = 0011                      (2 safe tiles)
 *            0011 SHL 1 = 0110
 *            0011 SHR 1 = 0001
 *                     XOR ----
 *                         0111 = .^^^  (1 safe tile)
 *
 * The total number of safe tiles for 2 rows would be 3. For part 2,
 * this is what we need to calculate for 400,000 rows of length 100.
 */

#include <stdio.h>   // fopen, fclose, printf
#include <stdint.h>  // uint64_t

// Union with overlapping members to easily access lower
// and higher 64-bit halves of the 128-bit integer.
typedef union {
    __uint128_t n;
    uint64_t a[2];
} UU128;

// Use custom compiler function to count bits in 64-bit halves of one 128-bit
// integer. 'Popcount' is not C standard but available in recent versions of
// gcc and clang. Tested with Debian gcc 10.2 on arm64 and Apple clang 14.0 on
// M1. Idea from https://stackoverflow.com/questions/55008994/most-efficient-popcount-on-uint128-t
static inline int bitcount(const __uint128_t n)
{
    const UU128 u = {n};
    return __builtin_popcountll(u.a[0]) + __builtin_popcountll(u.a[1]);
}

// Next rows of floor tiles, add up all safe tiles.
static int evolve(__uint128_t traps, const __uint128_t mask, const int rows)
{
    // Number of safe tiles (0) = number of traps (1) but with all floor tiles
    // reversed. I originally expressed this as safe = len - bitcount(traps)
    // but eliminated the whole 'len' after seeing Reddit user /u/askalski 's
    // almost identical solution at https://www.reddit.com/r/adventofcode/comments/5iyp50/2016_day_18_solutions/dbc0l6j/
    int safe = bitcount(traps ^ mask);
    for (int i = 1; i < rows; ++i) {
        // If the MSB of the current floor tiling is 1 (trap), then because of
        // shift-xor it would produce another 1 out of bounds, but all out of
        // bounds tiles must be 0 (safe), so use a bit mask to clear those.
        // Mask is 1 for floor tiles, 0 outside.
        traps = ((traps << 1) ^ (traps >> 1)) & mask;
        safe += bitcount(traps ^ mask);
    }
    return safe;
}

int main(void)
{
    __uint128_t traps = 0, mask = 0;
    int c;
    FILE *f = fopen("../aocinput/2016-18-input.txt", "r");
    if (!f)
        return 1;

    // Input is one line of 'traps' (^) and 'safe' tiles (.) no longer than
    // 128 tiles (in my case length=100). Object is to count safe tiles but
    // out of bounds tiles are also safe and because bit shift adds zeroes,
    // set safe=0 and trap=1. This slightly complicates counting safe tiles
    // but enables the shift-xor algorithm.
    // Originally I counted the length and set mask to (1<<len)-1.
    while ((c = fgetc(f)) >= '.') {  // better than "!='\n'" in case of EOF
        traps = (traps << 1) | (c == '^');
        mask  = (mask  << 1) | 1;
    }
    fclose(f);
    printf("Part 1: %d\n", evolve(traps, mask, 40    ));  // 1956
    printf("Part 2: %d\n", evolve(traps, mask, 400000));  // 19995121
    return 0;
}
