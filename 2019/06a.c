///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 6: Universal Orbit Map, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#include <stdio.h>   // fopen, fclose, getline
#include <stdlib.h>  // malloc, free
#include <ctype.h>   // isalnum

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Orbit {
	int parent, around, inorbit;
} ORBIT, *PORBIT;

const char *inp = "../aocinput/2019-06-input.txt";

////////// Functions //////////////////////////////////////////////////////////

// Count lines in a multi-line text file
int size(void)
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

// Read orbit lines from file to memory
// Arg: array a must be allocated
int read(PORBIT a, int n)
{
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int i, p, q, line = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while (line < n && getline(&s, &t, fp) > 0)
		{
			i = p = q = 0;
			while (i < t && isalnum(s[i]))
				p = p * 128 + s[i++];
			if (i < t && s[i] == ')')
			{
				++i;  // skip ')'
				while (i < t && isalnum(s[i]))
					q = q * 128 + s[i++];
			}
			a[line++] = (ORBIT){ -1, p, q };
		}
		free(s);
		fclose(fp);
	}
	return line;
}

void findparents(PORBIT a, int n)
{
	int i, j;

	for (i = 0; i < n; ++i)
		for (j = 0; j < n; ++j)
			if (i != j && a[j].inorbit == a[i].around)
			{
				a[i].parent = j;
				break;
			}
}

int countorbits(PORBIT a, int n)
{
	int i, p, k, sum = 0;

	for (i = 0; i < n; ++i)
	{
		p = i;
		k = 1;  // every entry already is 1 orbit!
		while ((p = a[p].parent) >= 0)
			++k;
		sum += k;
	}
	return sum;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	int len;
	PORBIT orb = NULL;

	if ((len = size()) > 0)
	{
		if ((orb = malloc(len * sizeof *orb)) != NULL)
		{
			if (read(orb, len) == len)
			{
				findparents(orb, len);
				printf("%d\n", countorbits(orb, len));
			}
			free(orb);
		}
	}
	return 0;
}
