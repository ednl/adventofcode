#include <stdio.h>
#include <stdlib.h>

const long long DECK = 119315717514047;
const long long MOVE = 101741582076661;
const long long START = 2020;

long long index = START, count = 0;
int key[100], val[100];

int main(void)
{
	// const long long DECK = 10007;
	// const long long MOVE = 1;
	// const long long START = 2019;
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int i, n;

	for (i = 0; i < 100; ++i)
	{
		key[i] = 0;
		val[i] = 0;
	}

	if ((fp = fopen("../aocinput/2019-22-input.txt", "r")) != NULL)
	{
		i = 0;
		while (i < 100 && getline(&s, &t, fp) > 0)
		{
			if (s[0] == 'c')
			{
				if ((n = atoi(&s[4])) > 0)
				{
					key[i] = 1;
					val[i] = n;
				} else if (n < 0)
				{
					key[i] = 2;
					val[i] = -n;
				}
			} else if (s[5] == 'i')
			{
				key[i] = 3;
				val[i] = 0;
			} else if (s[5] == 'w')
			{
				key[i] = 4;
				val[i] = atoi(&s[20]);
			}
			++i;
		}
		free(s);
		fclose(fp);
	}
	do
	{
		for (i = 0; i < 100; ++i)
		{
			switch (key[i])
			{
				case 1: index -= val[i]; if (index < 0) index += DECK; break;
				case 2: index += val[i]; if (index >= DECK) index -= DECK; break;
				case 3: index = DECK - 1 - index; break;
				case 4: index = index * val[i] % DECK; break;
			}
		}
		++count;
	} while (index != START && count < MOVE);
	printf("%lld %lld\n", count, index);
	return 0;
}
