#include <stdio.h>

#define TURNS 30000000u
static unsigned int seen[TURNS] = {0};

static const unsigned int game[] = {13, 16, 0, 12, 15, 1};
static const size_t gamesize = sizeof game / sizeof *game;

int main(void)
{
	unsigned int i, j, lastnum = game[gamesize - 1];

	for (i = 1; i < gamesize; ++i) {
		seen[game[i - 1]] = i;
	}
	for (; i < TURNS; ++i) {
		j = seen[lastnum];
		seen[lastnum] = i;
		lastnum = j ? i - j : 0;
	}

	printf("%u\n", lastnum);
	return 0;
}
