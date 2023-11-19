///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 10: Monitoring Station, part one & two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#include <stdio.h>   // fopen, fgetc, printf
#include <stdlib.h>  // atoi, atol
#include <math.h>    // sqrt, atan2

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Pos {
	int x, y, n, shot;
	double angle, dist;
} POS;

// Location of program on disk
//const char *inp = "../aocinput/2019-10-example.txt";
const char *inp = "../aocinput/2019-10-input.txt";

////////// Globals ////////////////////////////////////////////////////////////

static POS *asteroids = NULL;
static int *field = NULL;
static int width, depth, count;

////////// Functions //////////////////////////////////////////////////////////

// Count hashes in file
static int size(void)
{
	FILE *fp;
	int c, n = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while ((c = fgetc(fp)) != EOF)
			if (c == '#')
				++n;
		fclose(fp);
	}
	return n;
}

// Count lines in file
static int lines(void)
{
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int n = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while (getline(&s, &t, fp) > 0)
			++n;
		free(s);
		fclose(fp);
	}
	return n;
}

// Count characters in first line of file
static int chars(void)
{
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int n = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		if ((n = getline(&s, &t, fp)) > 0)
			while (n && (s[n - 1] == '\n' || s[n - 1] == '\r'))
				n--;
		free(s);
		fclose(fp);
	}
	return n;
}

// Read hash positions from file to memory
// Arg: a must be allocated
static int read(void)
{
	FILE *fp;
	int c, i = 0, p = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while ((c = fgetc(fp)) != EOF)
			if (c == '#' || c == '.')
			{
				if (c == '#')
					asteroids[p++] = (POS){ i % width, i / width, 0, 0, 0.0, 0.0 };
				field[i++] = (c == '#');
			}
		fclose(fp);
	}
	return p;
}

static int cansee(int x, int y, int sx, int sy, int sn)
{
	int i, test = 1;

	for (i = 1; i < sn; ++i)  // number of steps to integer intermediate points
	{
		x += sx;  // next intermediate point
		y += sy;
		if (field[y * width + x])  // asteroid in the way?
		{
			test = 0;  // we can't see asteroid j from i
			break;     // no point in looking further
		}
	}
	return test;
}

static int cmp(const void *a, const void *b)
{
	double p, q;

	p = ((const POS *)a)->angle;
	q = ((const POS *)b)->angle;
	if (p < q)
		return -1;
	if (p > q)
		return 1;
	p = ((const POS *)a)->dist;
	q = ((const POS *)b)->dist;
	if (p < q)
		return -1;
	if (p > q)
		return 1;
	return 0;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	int i, j, k, dx, dy, t, stepx, stepy, steps, max = 0, imax = 0, min;
	double f;

	width = chars();
	depth = lines();
	count = size();
	if (width > 0 && depth > 0 && count > 0)
	{
		asteroids = malloc(count * sizeof *asteroids);
		field = malloc(width * depth * sizeof *field);
		if (asteroids != NULL && field != NULL)
		{
			if (read() == count)
			{
				// Print field
				k = 0;
				for (i = 0; i < depth; ++i)
				{
					for (j = 0; j < width; ++j)
						printf("%c", field[k++] ? '#' : '.');
					printf("\n");
				}
				// Data is good
				for (i = 0; i < count; ++i)  // test every asteroid
				{
					for (j = 0; j < count; ++j)
					{
						if (i != j)  // compare with every other asteroid
						{
							// get common factors from dx, dy
							dx = asteroids[j].x - asteroids[i].x;  // from i to j
							dy = asteroids[j].y - asteroids[i].y;
							if (abs(dx) > 1 || abs(dy) > 1)
							{
								if (!dx || !dy)
								{
									// horizontal or vertical
									stepx = dx ? dx / abs(dx) : 0;
									stepy = dy ? dy / abs(dy) : 0;
									steps = dx ? abs(dx) : abs(dy);
									t = cansee(asteroids[i].x, asteroids[i].y, stepx, stepy, steps);
									asteroids[i].n += t;
								} else if (abs(dx) == abs(dy))
								{
									// diagonal
									stepx = dx / abs(dx);
									stepy = dy / abs(dy);
									steps = abs(dx);
									t = cansee(asteroids[i].x, asteroids[i].y, stepx, stepy, steps);
									asteroids[i].n += t;
								} else
								{
									// at an angle
									t = 1;  // premise is: yes, we see j from i
									min = abs(dx) < abs(dy) ? abs(dx) : abs(dy);
									for (k = min; k >= 2; --k)
									{
										if (!(dx % k) && !(dy % k))  // must be integer steps
										{
											stepx = dx / k;  // integer steps from asteroid i to j
											stepy = dy / k;
											steps = k;
											t = t && cansee(asteroids[i].x, asteroids[i].y, stepx, stepy, steps);
											if (!t)
												break;
										}
									}
									if (t)  // no intermediate asteroid?
										asteroids[i].n++;  // asteroid j can be seen from i
								}
							} else
								// too close, no intermediate points
								asteroids[i].n++;
						}
					}
					if (asteroids[i].n > max)
					{
						max = asteroids[i].n;
						imax = i;
					}
				}
				printf("max %d(%d,%d) = %d\n", imax, asteroids[imax].x, asteroids[imax].y, max);
				// Compute distance and angle from the observation point for every asteroid
				for (i = 0; i < count; ++i)
				{
					if (i == imax)
						continue;
					dx = asteroids[i].x - asteroids[imax].x;
					dy = asteroids[i].y - asteroids[imax].y;
					asteroids[i].dist = sqrt(dx * dx + dy * dy);
					f = atan2(dy, dx) + M_PI / 2;
					if (f < 0)
						f += M_PI * 2;
					asteroids[i].angle = f;
				}
				qsort(asteroids, count, sizeof *asteroids, cmp);
				f = 0;
				for (i = 0; i < count; ++i)
				{
					if (asteroids[i].angle > f && asteroids[i].angle - f < 0.001)
						puts("----------------------------");
					f = asteroids[i].angle;
					printf("%3d: %2d,%2d %5.3f %6.3f\n", i, asteroids[i].x, asteroids[i].y, asteroids[i].angle, asteroids[i].dist);
				}
				// Shoot
				k = 0;  // number of shots
				i = 1;  // index of asteroid in the crosshairs
				while (k < 200 && k < count - 1)
				{
					// next one not shot yet
					while (asteroids[i].shot)
						i = (i % (count - 1)) + 1;
					++k;
					printf("%3d: %3d %2d,%2d %5.3f %6.3f\n", k, i, asteroids[i].x, asteroids[i].y, asteroids[i].angle, asteroids[i].dist);
					if (k == 200) {
						// found the 200th shot
						printf("%d\n", 100 * asteroids[i].x + asteroids[i].y);
						break;
					}
					asteroids[i].shot = 1;
					// skip all asteroids on the same angle
					f = asteroids[i].angle;
					i = (i % (count - 1)) + 1;
					while (fabs(asteroids[i].angle - f) < 1e-6) {
						i = (i % (count - 1)) + 1;
					}
				}
			}
		}
		free(field);
		free(asteroids);
	}
	return 0;
}
