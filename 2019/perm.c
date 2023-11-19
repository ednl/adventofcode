#include <stdio.h>   // printf

void perm1(void)
{
	int a, b, c, d, e;

	for (a = 0; a < 5; ++a)
	{
		for (b = 0; b < 5; ++b)
		{
			if (b == a)
				continue;
			for (c = 0; c < 5; ++c)
			{
				if (c == a || c == b)
					continue;
				for (d = 0; d < 5; ++d)
				{
					if (d == a || d == b || d == c)
						continue;
					for (e = 0; e < 5; ++e)
					{
						if (e == a || e == b || e == c || e == d)
							continue;
						printf("%d%d%d%d%d\n", a, b, c, d, e);
					}
				}
			}
		}
	}
}

void perm2(int n, int k)
{
	static int u[] = {0,0,0,0,0,0,0,0,0,0};  // index of used elements
	static int r[] = {0,0,0,0,0,0,0,0,0,0};  // permutation result
	static int p = 0;
	int i;

	for (i = 0; i < n; ++i)
	{
		if (u[i])             // is element i already in use?
			continue;         // if so, skip it
		r[k] = i;             // add unused element to the permutation
		if (k < n - 1)        // more positions to go?
		{
			u[i] = 1;         // element i is now in use
			perm2(n, k + 1);  // go to the next position
			u[i] = 0;         // element i is no longer in use
		} else                // this permutation is ready
		{
			// Do something with the permutation
			// (must be done here because recursive function
			// still only returns once to parent)
			printf("%d: ", p++);
			for (i = 0; i < n; ++i)
				printf("%d", r[i]);
			printf("\n");
		}
	}
}

int main(void)
{
	perm2(5, 0);
	return 0;
}
