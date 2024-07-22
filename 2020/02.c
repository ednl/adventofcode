#include <stdio.h>

int main(void)
{
    FILE *f = fopen("../aocinput/2020-02-input.txt", "r");
    if (!f)
        return 1;
    int valid1 = 0, valid2 = 0;
    int min, max;
    char test, passw[32];
    while (fscanf(f, "%d-%d %c: %31s", &min, &max, &test, passw) == 4) {
        int count = 0;
        for (int i = 0; passw[i]; ++i)
            count += passw[i] == test;
        valid1 += min <= count && count <= max;
        valid2 += (passw[min - 1] == test) ^ (passw[max - 1] == test);
    }
    fclose(f);
    printf("Part 1: %d\n", valid1);  // 500
    printf("Part 2: %d\n", valid2);  // 313
    return 0;
}
