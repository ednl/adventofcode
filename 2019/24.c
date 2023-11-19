#include <stdio.h>
#include <stdlib.h>

#define DIM 5
#define LEN (DIM * DIM)
#define BITS (8 * sizeof(unsigned int))

typedef unsigned int uint;

uint grid[LEN], next[LEN];
uint hist[(1u << LEN) / BITS];

uint neighbours(uint *a, uint i)
{
	uint x = i % DIM, y = i / DIM, sum = 0;

	if (x > 0)       sum += a[i - 1];
	if (x < DIM - 1) sum += a[i + 1];
	if (y > 0)       sum += a[i - DIM];
	if (y < DIM - 1) sum += a[i + DIM];
	return sum;
}

void evolve(uint *cur, uint *nxt)
{
	uint i, n;

	for (i = 0; i < LEN; ++i)
	{
		n = neighbours(cur, i);
		nxt[i] = cur[i] ? n == 1 : n == 1 || n == 2;
	}
}

uint biodiv(uint *a)
{
	uint i, sum = 0, bit = 1;

	for (i = 0; i < LEN; ++i)
	{
		sum += bit * a[i];
		bit *= 2;
	}
	return sum;
}

uint repeat(uint id)
{
	uint n = id / BITS;
	uint mask = 1u << (id % BITS);

	if (hist[n] & mask)
		return 1u;
	hist[n] |= mask;
	return 0u;
}

int main(void)
{
	FILE *fp;
	int c;
	uint i, *a, *b, *t;

	for (i = 0; i < LEN; ++i)
		grid[i] = 0;

	if ((fp = fopen("../aocinput/2019-24-input.txt", "r")) != NULL)
	{
		i = 0;
		while ((c = fgetc(fp)) != EOF)
			if (c == '#' || c == '.')
				grid[i++] = c == '#';
		fclose(fp);
	}

	a = grid;
	b = next;
	while (!repeat((i = biodiv(a))))
	{
		evolve(a, b);
		t = a;
		a = b;
		b = t;
		++i;
	}
	printf("%u\n", i);

	return 0;
}
