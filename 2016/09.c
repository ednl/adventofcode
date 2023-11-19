#include <stdio.h>   // fopen, getline, sscanf, printf, fclose
#include <stdlib.h>  // free

static size_t decompress(const char *c, const char * const end, const char recurse)
{
    size_t size = 0;
    unsigned int len, rep;
    while (c < end) {
        while (c < end && *c++ != '(')
            ++size;
        if (c < end && sscanf(c, "%ux%u", &len, &rep) == 2) {
            while (*c++ != ')');  // skip over the marker
            size += recurse ? decompress(c, c + len, 1) * rep : len * rep;
            c += len;
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
        printf("Part 1: %zu\n", decompress(line, line + len, 0));  // 123908
        printf("Part 2: %zu\n", decompress(line, line + len, 1));  // 10755693147
        free(line);
    }
    fclose(f);
    return 0;
}
