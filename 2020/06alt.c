#include <stdio.h>
#include <string.h>  // memset

#define LINELEN 32
#define LEN ('z' - 'a' + 1)

static char yes[LEN];
static int any, all, groupsize;

static void count(void)
{
    for (int i = 0; i < LEN; ++i) {
        any += yes[i] != 0;
        all += yes[i] == groupsize;
    }
    groupsize = 0;
    memset(yes, 0, LEN);
}

int main(void)
{
    FILE *f = fopen("../aocinput/2020-06-input.txt", "r");
    if (!f)
        return 1;
    char buf[LINELEN];
    while (fgets(buf, sizeof buf, f)) {
        int i = 0;
        for (const char *c = buf; *c != '\n'; ++c, ++i)
            yes[*c - 'a']++;
        if (i)
            ++groupsize;
        else
            count();
    }
    fclose(f);
    count();

    printf("Part 1: %d\n", any);  // 6587
    printf("Part 2: %d\n", all);  // 3235
    return 0;
}
