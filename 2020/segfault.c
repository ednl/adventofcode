#include <stdio.h>
#define FNAME "../aocinput/2020-09-input.txt"
#define FSIZE 10000  // needed for my input: 8824
#define LEN   1000   // lines in input file
static char input[FSIZE];
static int data[LEN];
static int parseint(const char **s)
{
    int x = *(*s)++ & 15;  // use unsigned here to avoid segfault
    while (**s != '\n')
        // signed int overflow segfaults
        // on RPiOS Debian "Bookworm" 12.14
        // when compiled with gcc 12.2
        // using -O2 or -O3, but not -O1
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}
int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f)
        return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);
    int n = 0;
    for (const char *c = input; (data[n] = parseint(&c)) > 0; ++n)
        ;
    for (int i = n - 1; i < n + 2; ++i)
        printf("data[%d]=%d\n", i, data[i]);
}
