///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 24: Planet of Discord, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#include <stdio.h>

#define DIM 5
#define STEPS 200

#define MID (DIM / 2)          // midpoint
#define AREA (DIM * DIM)       // number of tiles in one level
#define LEVELS (STEPS + 1)     // maximum number of levels (positive + zero + negative)
#define TILES (LEVELS * AREA)  // total number of tiles
#define LEVELMAX (STEPS / 2)
#define LEVELMIN (-LEVELMAX)

////////// Function Definitions ///////////////////////////////////////////////

int ix(int level, int x, int y)
{
	if (level < LEVELMIN || level > LEVELMAX || x < 0 || x >= DIM || y < 0 || y >= DIM)
		return -1;

	return (level - LEVELMIN) * AREA + y * DIM + x;
}

void printlevels(int *a)
{
	int n, m, x, y, i;
	
	for (m = LEVELMIN; m <= LEVELMAX; m += 26)
	{
		for (y = 0; y < DIM; ++y)
		{
			for (n = m; n < m + 26 && n <= LEVELMAX; ++n)
			{
				for (x = 0; x < DIM; ++x)
				{
					if (x == MID && y == MID)
						printf("?");
					else if ((i = ix(n, x, y)) >= 0)
						printf("%c", a[i] ? '#' : '.');
				}
				printf(" ");
			}
			printf("\n");
		}
		printf("\n");
	}
}

int bugrow(int *a, int level, int row)
{
	int x, i, sum = 0;

	if (level > LEVELMAX)
		return 0;  // out of range

	for (x = 0; x < DIM; ++x)
		if ((i = ix(level, x, row)) >= 0)
			sum += a[i];

	return sum;
}

int bugcol(int *a, int level, int col)
{
	int y, i, sum = 0;

	if (level > LEVELMAX)
		return 0;  // out of range

	for (y = 0; y < DIM; ++y)
		if ((i = ix(level, col, y)) >= 0)
			sum += a[i];

	return sum;
}

int bug(int *a, int level, int x, int y)
{
	int i;

	if (level < LEVELMIN)
		return 0;  // out of range

	if (x == -1)
		return bug(a, level - 1, MID - 1, MID);
	if (x == DIM)
		return bug(a, level - 1, MID + 1, MID);
	if (y == -1)
		return bug(a, level - 1, MID, MID - 1);
	if (y >= DIM)
		return bug(a, level - 1, MID, MID + 1);

	if ((i = ix(level, x, y)) >= 0)
		return a[i];

	return 0;
}

int neighbours(int *a, int level, int x, int y)
{
	int x1, y1, sum = 0;

	if (level < LEVELMIN || level > LEVELMAX)
		return 0;  // out of range

	x1 = x; y1 = y + 1;  // below
	sum += x1 == 2 && y1 == 2 ? bugrow(a, level + 1, 0)       : bug(a, level, x1, y1);

	x1 = x; y1 = y - 1;  // above
	sum += x1 == 2 && y1 == 2 ? bugrow(a, level + 1, DIM - 1) : bug(a, level, x1, y1);

	x1 = x + 1; y1 = y;  // right
	sum += x1 == 2 && y1 == 2 ? bugcol(a, level + 1, 0)       : bug(a, level, x1, y1);

	x1 = x - 1; y1 = y;  // left
	sum += x1 == 2 && y1 == 2 ? bugcol(a, level + 1, DIM - 1) : bug(a, level, x1, y1);

	return sum;
}

void evolve(int *cur, int *nxt)
{
	int i, j, x, y, n;

	for (i = LEVELMIN; i <= LEVELMAX; ++i)
		for (y = 0; y < DIM; ++y)
			for (x = 0; x < DIM; ++x)
				if ((x != MID || y != MID) && (j = ix(i, x, y)) >= 0)
				{
					n = neighbours(cur, i, x, y);
					nxt[j] = cur[j] ? n == 1 : n == 1 || n == 2;
				}
}

int count(int *a)
{
	int n, x, y, i = 0, sum = 0;

	for (n = LEVELMIN; n <= LEVELMAX; ++n)
		for (y = 0; y < DIM; ++y)
			for (x = 0; x < DIM; ++x)
			{
				if (x != MID || y != MID)
					sum += a[i];
				++i;
			}
	return sum;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	FILE *fp;
	int c, i, j, x, y, sum;
	int grid[TILES], next[TILES];
	int *a, *b, *t;

	for (i = 0; i < TILES; ++i)
		grid[i] = 0;

	if ((fp = fopen("../aocinput/2019-24-input.txt", "r")) != NULL)
	{
		i = ix(0, 0, 0);
		while ((c = fgetc(fp)) != EOF)
			if (c == '#' || c == '.' || c == '?')
				grid[i++] = c == '#';
		fclose(fp);
	}

	printf("\033[?25l");  // hide cursor
	printf("\033[2J\033[1;1H");  // cls-home
	printf("minute %3d bugs %4d\n\n", 0, count(grid));
	printlevels(grid);

	a = grid;
	b = next;
	for (i = 0; i < STEPS; ++i)
	{
		evolve(a, b);
		t = a;
		a = b;
		b = t;
		printf("\033[1;1H");  // home
		printf("minute %3d bugs %4d\n\n", i + 1, count(a));
		printlevels(a);
	}
	printf("\033[?25h");  // show cursor
	return 0;
}
