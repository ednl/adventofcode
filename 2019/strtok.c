#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strtok

int main(void)
{
	char s[] = "7 JKRV, 3 FKTLR, 19 FDSBZ => 9 HPRPM\n";
	const char *d = " ,=>\n";

	int n = 0;
	char *t = strtok(s, d);
	while (t) {
		int id = 0;
		if (n & 1) {
			char *c = t;
			while (*c)
				id = (id << 5) | (*c++ - 'A');
		} else
			id = atoi(t);
		printf("%d: %s = %d\n", n++, t, id);
		t = strtok(NULL, d);
	}

	return 0;
}
