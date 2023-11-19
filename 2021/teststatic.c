#include <stdio.h>

static void foo(void)
{
	static int var = 0;
	printf("%d\n", ++var);
}

int main(void)
{
	foo();
	foo();
	return 0;
}
