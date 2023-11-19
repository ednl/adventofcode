#include <stdio.h>

#define N 1000
typedef struct {
    int cmd, arg;
} Nav;
static Nav nav[N] = {0};

int main(void)
{
    FILE *f = fopen("../aocinput/2021-02-input.txt", "r");
    if (!f)
        return 1;

    int i = 0, arg;
    char cmd[10];
    while (i < N && fscanf(f, "%9s %d ", cmd, &arg) == 2) {
        nav[i++] = (Nav){cmd[0], arg};
    }
    fclose(f);
    if (i != N)
        return 2;

    int fwd = 0, depth = 0;
    for (i = 0; i < N; ++i) {
        switch (nav[i].cmd) {
            case 'f': fwd += nav[i].arg; break;
            case 'd': depth += nav[i].arg; break;
            case 'u': depth -= nav[i].arg; break;
        }
    }
    printf("Part 1: %d\n", fwd * depth);

    fwd = depth = 0;
    int aim = 0;
    for (i = 0; i < N; ++i) {
        switch (nav[i].cmd) {
            case 'f': fwd += nav[i].arg; depth += aim * nav[i].arg; break;
            case 'd': aim += nav[i].arg; break;
            case 'u': aim -= nav[i].arg; break;
        }
    }
    printf("Part 2: %d\n", fwd * depth);
    return 0;
}
