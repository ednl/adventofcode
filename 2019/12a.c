///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 12: The N-Body Problem, part one
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#define DIM 3

typedef int VEC[DIM];

typedef struct Body {
	VEC pos, vel;
} BODY, *PBODY;

BODY moon[] = {
	{{-7, 17, -11}, {0, 0, 0}},
	{{ 9, 12,   5}, {0, 0, 0}},
	{{-9,  0,  -4}, {0, 0, 0}},
	{{ 4,  6,   0}, {0, 0, 0}}
};
const int n = sizeof moon / sizeof *moon;

void velocity(PBODY a, PBODY b)
{
	int i, d;

	for (i = 0; i < DIM; ++i)
		if ((d = a->pos[i] - b->pos[i]))
		{
			d /= abs(d);
			a->vel[i] += -d;
			b->vel[i] += d;
		}
}

void position(PBODY a)
{
	int i;

	for (i = 0; i < DIM; ++i)
		a->pos[i] += a->vel[i];
}

int vec_energy(VEC a)
{
	int i, sum = 0;

	for (i = 0; i < DIM; ++i)
		sum += abs(a[i]);
	return sum;
}

int body_energy(PBODY a)
{
	return vec_energy(a->pos) * vec_energy(a->vel);
}

int main(void)
{
	int i, j, t, tot = 0;
	PBODY m = moon;

	printf("Step 0:\n");
	for (i = 0; i < n; ++i)
		printf("%d: (%3d,%3d,%3d) (%3d,%3d,%3d)\n", i,
			moon[i].pos[0], moon[i].pos[1], moon[i].pos[2],
			moon[i].vel[0], moon[i].vel[1], moon[i].vel[2]);
	printf("\n");

	for (t = 1; t <= 1000; ++t)
	{
		for (i = 0; i < n - 1; ++i)
			for (j = i + 1; j < n; ++j)
				velocity(&moon[i], &moon[j]);
		for (i = 0; i < n; ++i)
			position(&moon[i]);

		printf("Step %d:\n", t);
		for (i = 0; i < n; ++i)
			printf("%d: (%3d,%3d,%3d) (%3d,%3d,%3d)\n", i,
				moon[i].pos[0], moon[i].pos[1], moon[i].pos[2],
				moon[i].vel[0], moon[i].vel[1], moon[i].vel[2]);
		printf("\n");
	}

	for (i = 0; i < n; ++i)
		tot += body_energy(m++);
	printf("Total energy: %d\n", tot);

	return 0;
}
