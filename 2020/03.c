#include <stdio.h>   // printf
#include <stdlib.h>  // malloc, getline
#include <string.h>  // memcpy

static const char *inp = "../aocinput/2020-03-input.txt";
static const int right[] = {1,3,5,7,1};
static const int down[]  = {1,1,1,1,2};

// Count lines in file
int lines(void)
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
int chars(void)
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

// Read file into array
int read(char *a, int len)
{
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int n = 0;
	char *src, *dst = a;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while (getline(&s, &t, fp) > 0) {
			src = s;
			while (*src != '\n' && *src != '\r' && *src != '\0' && n < len) {
				*dst++ = *src++;
				++n;
			}
		}
		free(s);
		fclose(fp);
	}
	return n;
}

int main(void)
{
	int i, j, k;
	int rows = lines();
	int cols = chars();
	int size = rows * cols;
	char *grid = malloc(size * sizeof *grid);

	// Check sizes
	printf("lines:%6d\n", rows);
	printf("chars:%6d\n", cols);
	printf("size :%6d\n", size);
	printf("read :%6d\n", read(grid, size));
	printf("\n");

	// Check grid
	printf("    ");
	for (j = 1; j <= cols; ++j)
		printf("%d", j % 10);
	printf("\n");
	int show = 10;
	k = 0;
	for (i = 0; i < rows; ++i) {
		if (i < show / 2 || i >= rows - show / 2) {
			printf("%3d ", i + 1);
			for (j = 0; j < cols; ++j) {
					printf("%c", grid[k]);
				++k;
			}
			printf("\n");
		} else {
			if (i == show / 2)
				printf("...\n");
			k += cols;
		}
	}
	printf("\n");

	int x, y, trees, slopes = sizeof right / sizeof *right;
	unsigned long prod = 1;
	for (i = 0; i < slopes; ++i) {
		trees = x = y = k = 0;
		while (k < size) {
			if (grid[k] == '#')
				++trees;
			x = (x + right[i]) % cols;
			y += down[i];
			k = y * cols + x;
		}
		printf("slope (%d,%d) = %3d", right[i], down[i], trees);
		if (right[i] == 3 && down[i] == 1)
			printf(" <- part 1");
		printf("\n");
		prod *= trees;
	}
	printf("\npart 2: %lu\n", prod);

	free(grid);
	return 0;
}
