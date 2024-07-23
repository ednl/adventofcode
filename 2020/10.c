#include <stdio.h>
#include <stdlib.h>

#define INPUTLEN 100  // number of lines in the input
#define DATALEN (INPUTLEN + 2)  // 2 extra elements

static const char *fname = "../aocinput/2020-10-input.txt";
static int data[DATALEN];
static long *path = NULL;

// Comparison function for qsort() of int array
int cmp(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Assumes data is sorted
int indata(const int k)
{
    for (int i = 0; i < DATALEN; ++i) {
        if (data[i] > k)
            return 0;
        if (data[i] == k)
            return 1;
    }
    return 0;
}

int main(void)
{
    FILE *fp;
	char *s = NULL;
	size_t t = 0;
    int i = 0, j, pathlen;

	if ((fp = fopen(fname, "r"))) {
		while (i < INPUTLEN && getline(&s, &t, fp) > 0)
			data[i++] = atoi(s);
		free(s);
		fclose(fp);
	}

    data[DATALEN - 2] = 0; // add zero as second-to-last element
    qsort(data, DATALEN - 1, sizeof *data, cmp);  // sort without last element
    data[DATALEN - 1] = data[DATALEN - 2] + 3;  // set last element to max+3

    pathlen = data[DATALEN - 1] + 1;
    path = malloc(pathlen * sizeof *path);
    if (path) {
        path[0] = 1;
        for (i = 1; i < pathlen; ++i) {
            path[i] = 0;
            for (j = i - 3; j < i; ++j)
                if (indata(j))
                    path[i] += path[j];
        }
    }
    printf("%ld\n", path[pathlen - 1]);
    free(path);
    return 0;
}
