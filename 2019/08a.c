///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 8: Space Image Format, part one
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#define W 25
#define H 6
#define P (W * H)

int main(void)
{
	FILE *fp;
	int i = 0, c, k, n[] = {0,0,0}, min = -1, res = -1;

	if ((fp = fopen("../aocinput/2019-08-input.txt", "r")) != NULL)
	{
		while ((c = fgetc(fp)) != EOF)
		{
			if ((k = c - '0') >= 0 && k < 3)
			{
				n[k]++;  // count number of 0's, 1's and 2's
				i++;     // total valid characters counted
				if (i == P)
				{
					// next page
					if (min == -1 || n[0] < min)
					{
						min = n[0];
						res = n[1] * n[2];
					}
					for (i = 0; i < 3; ++i)
						n[i] = 0;
					i = 0;
				}
			}
		}
	}
	printf("%d\n", res);

	return 0;
}
