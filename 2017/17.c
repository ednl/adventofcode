#include <stdio.h>

#define BUFSIZE (2018)
#define STEP     (303)

static int buf[BUFSIZE] = {0};

int main(void)
{
    // Part 1
    int i, len = 0, pos = 0;
    while (++len < BUFSIZE) {
        pos = (pos + STEP) % len + 1;
        for (i = len; i > pos; --i) {
            buf[i] = buf[i - 1];
        }
        buf[pos] = len;
    }
    printf("%i\n", buf[++pos % BUFSIZE]);

    // Part 2
    int last = 50 * 1000 * 1000;
    i = len = pos = 0;
    while (++len <= last) {
        pos = (pos + STEP) % len + 1;
        if (pos == 1) {
            i = len;
        }
    }
    printf("%i\n", i);

    return 0;
}
