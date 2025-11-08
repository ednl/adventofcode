///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 12: The N-Body Problem, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define N   4
#define DIM 3
#define LEN (N * DIM)

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Body {
    int64_t pos, vel;
} BODY;

////////// Globals ////////////////////////////////////////////////////////////

// Example 1
// Energy after 10: 179
// Repeats after: 2772
// <x=-1, y=0, z=2>
// <x=2, y=-10, z=-7>
// <x=4, y=-8, z=8>
// <x=3, y=5, z=-1>
// static BODY part[LEN] = {
// 	{-1,0},{  2,0},{ 4,0},{ 3,0},
// 	{ 0,0},{-10,0},{-8,0},{ 5,0},
// 	{ 2,0},{ -7,0},{ 8,0},{-1,0}};

// Example 2
// Energy after 100: 1940
// Repeats after: 4686774924
// <x=-8, y=-10, z=0>
// <x=5, y=5, z=10>
// <x=2, y=-7, z=3>
// <x=9, y=-8, z=-3>
// static BODY part[LEN] = {
// 	{ -8, 0},{ 5, 0},{ 2, 0},{ 9, 0},
// 	{-10, 0},{ 5, 0},{-7, 0},{-8, 0},
// 	{  0, 0},{10, 0},{ 3, 0},{-3, 0}};

// Puzzle
// Energy after 1000: 7013
// Repeats after: 324618307124784
//   x: 186028 = 2 2                        46507
//   y: 193052 = 2 2       17 17        167
//   z: 144624 = 2 2 2 2 3       23 131
//   LCM       = 2 2 2 2 3 17 17 23 131 167 46507
// <x=-7, y=17, z=-11>
// <x=9, y=12, z=5>
// <x=-9, y=0, z=-4>
// <x=4, y=6, z=0>
static BODY part[LEN] = {
    { -7, 0},{ 9, 0},{-9, 0},{4, 0},
    { 17, 0},{12, 0},{ 0, 0},{6, 0},
    {-11, 0},{ 5, 0},{-4, 0},{0, 0}};

static BODY init[N];

////////// Functions //////////////////////////////////////////////////////////

// Next time step in one dimension
// dim = 0..DIM-1
static void evolve(int dim)
{
    int i, j, p0 = N * dim, pn = N * (dim + 1);
    int64_t d;

    for (i = p0; i < pn - 1; ++i)
        for (j = i + 1; j < pn; ++j)
            if ((d = part[i].pos - part[j].pos))
            {
                d = d > 0 ? 1 : -1;
                part[i].vel -= d;
                part[j].vel += d;
            }
    for (i = p0; i < pn; ++i)
        part[i].pos += part[i].vel;
}

// Energy of moon i
// i = 0..N-1
static int64_t energy(int n)
{
    int i;
    int64_t ep = 0, ek = 0;

    for (i = 0; i < LEN; i += N)
    {
        ep += llabs(part[n + i].pos);  // potential energy
        ek += llabs(part[n + i].vel);  // kinetic energy
    }
    return ep * ek;
}

// Save starting point of one dimension
// dim = 0..DIM-1
static void save(int dim)
{
    int i;

    dim *= N;
    for (i = 0; i < N; ++i)
        init[i] = part[dim + i];
}

// Compare one dimension of one moon to starting point
// i = 0..N-1
// j = 0..LEN-1
static int samepoint(int i, int j)
{
    return init[i].pos == part[j].pos && init[i].vel == part[j].vel;
}

// Compare one dimension of all moons to starting point
// dim = 0..DIM-1
static int sameconfig(int dim)
{
    int i;

    dim *= N;
    for (i = 0; i < N; ++i)
        if (!samepoint(i, dim + i))
            return 0;
    return 1;
}

// Greatest Common Divisor
static int64_t gcd(int64_t a, int64_t b)
{
    int64_t t;
    while (b)
    {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// Least (Lowest) Common Multiple
static int64_t lcm(int64_t a, int64_t b)
{
    return a / gcd(a, b) * b;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    int i, dim, repeat[DIM] = {0,0,0};
    int64_t res;

    // 1000 generations for every dimension
    for (dim = 0; dim < DIM; ++dim)
        for (i = 0; i < 1000; ++i)
            evolve(dim);

    // Total energy of complete constellation
    res = 0;
    for (i = 0; i < N; ++i)
        res += energy(i);
    printf("%lld\n", res);

    // Dimensions repeat after how many steps?
    // (bijective so any starting point is good)
    for (dim = 0; dim < DIM; ++dim)
    {
        save(dim);
        do {
            evolve(dim);
            repeat[dim]++;
        } while (!sameconfig(dim));
    }

    // Dimensions are independent, so
    // result is LCM of steps per dimension
    res = repeat[0];
    for (i = 1; i < DIM; ++i)
        res = lcm(res, repeat[i]);
    printf("%lld\n", res);

    return 0;
}
