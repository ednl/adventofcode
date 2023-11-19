#include <stdio.h>     // fopen, fclose, fgetc, getdelim, getline, printf
#include <stdlib.h>    // atoi, atol(l), malloc, realloc, free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64

#define ERR_OK               0
#define ERR_OUTOFMEMORY      7
#define VM_MEMUNIT        4096

int vm_grow(int64_t **mem, int *size)
{
	int64_t *p = *mem;
	int n = *size, m = n + VM_MEMUNIT;

	if ((p = realloc(p, m * sizeof *p)) != NULL)
	{
		while (n < m)
			p[n++] = 0;
		*mem = p;
		*size = m;
		return ERR_OK;
	} else
		return ERR_OUTOFMEMORY;
}

int main(void)
{
	int i, n = 0;
	int64_t *mem = NULL;

	for (i = 0; i < 4; ++i)
	{
		vm_grow(&mem, &n);
		printf("mem=%08x n=%d\n", (unsigned int)mem, n);
	}

	free(mem);
	return 0;
}
