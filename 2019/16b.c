///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 16: Flawed Frequency Transmission, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#include <stdio.h>   // fopen, fgetc, printf
#include <stdlib.h>  // abs
#include <ctype.h>   // isdigit

#define MAXLEN 1000

////////// Function Declarations //////////////////////////////////////////////

int digits(const char * const, int *);
void show(int *, int);

////////// Function Definitions ///////////////////////////////////////////////

int digits(const char * const s, int *a)
{
	FILE *fp;
	const char * pc;
	int c, i = 0;

	if ((fp = fopen(s, "r")) != NULL)
	{
		while (i < MAXLEN && (c = fgetc(fp)) != EOF)
			if (isdigit(c))
				a[i++] = c - '0';
		fclose(fp);
	} else
	{
		pc = s;
		while (i < MAXLEN && (c = *pc++))
			if (isdigit(c))
				a[i++] = c - '0';
	}
	return i;
}

void show(int *a, int n)
{
	int i;

	for (i = 0; i < n; ++i)
		printf("%d", a[i]);
	printf("\n");
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	int i, phase, len, size, offset = 0;
	int data[MAXLEN], *vec;

	// Get data from file
	if (!(len = digits("../aocinput/2019-16-input.txt", data)))
		return 1;

	// Get offset from data
	for (i = 0; i < 7; ++i)
		offset = offset * 10 + data[i];

	// Storage space from offset onwards to 10000 * length of input
	size = 10000 * len - offset;
	if ((vec = malloc(size * sizeof *vec)) == NULL)
		return 1;

	// Initialise vector with data from offset
	for (i = 0; i < size; ++i)
		vec[i] = data[offset++ % len];

	// 100 Repeated phases
	for (phase = 0; phase < 100; ++phase)
		// Reverse to avoid O(n!)
		for (i = size - 2; i >= 0; --i)
			// Each element is the sum for the next one
			vec[i] = (vec[i] + vec[i + 1]) % 10;

	show(vec, 8);
	free(vec);
	return 0;
}
