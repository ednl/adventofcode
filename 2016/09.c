#include <stdio.h>   // fopen, getline, sscanf, printf, fclose
#include <stdlib.h>  // free
#include <stdbool.h>
#include "../startstoptimer.h"

static size_t decompress(const char *beg, const char *const end, const bool recurse)
{
    size_t size = 0;
    unsigned int len, rep;
    while (beg < end) {
        while (beg < end && *beg++ != '(')
            ++size;
        if (beg < end && sscanf(beg, "%ux%u", &len, &rep) == 2) {
            while (*beg++ != ')');  // skip over the marker
            size += rep * (recurse ? decompress(beg, beg + len, true) : len);
            beg += len;
        }
    }
    return size;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-09-input.txt", "r");
    if (!f)
        return 1;
    char *line = NULL;
    size_t size;
    ssize_t len;
    if ((len = getline(&line, &size, f)) > 0) {
        line[--len] = '\0';  // strip EOL
        starttimer();
        printf("Part 1: %zu\n", decompress(line, line + len, false));  // 123908
        printf("Part 2: %zu\n", decompress(line, line + len, true));   // 10755693147
        printf("Time: %.0f us\n", stoptimer_ms());
        free(line);
    }
    fclose(f);
    return 0;
}
