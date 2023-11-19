#include <stdio.h>

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "../aocinput/2016-06-example.txt"
#define N 6
#else
#define NAME "../aocinput/2016-06-input.txt"
#define N 8
#endif

#define L ('z' - 'a' + 1)

static int hist[N][L];

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[16];
    int lines = 0;
    while (fgets(buf, sizeof buf, f)) {
        ++lines;
        for (int i = 0; i < N; ++i)
            hist[i][buf[i] - 'a']++;
    }
    fclose(f);
    char message1[N + 1] = {0};
    char message2[N + 1] = {0};
    for (int i = 0; i < N; ++i) {
        int max = 0;
        int min = lines;
        for (int j = 0; j < L; ++j) {
            if (hist[i][j] > max) {
                max = hist[i][j];
                message1[i] = (char)(j + 'a');
            }
            if (hist[i][j] && hist[i][j] < min) {
                min = hist[i][j];
                message2[i] = (char)(j + 'a');
            }
        }
    }
    printf("Part 1: %s\n", message1);  // example = easter, input = qrqlznrl
    printf("Part 2: %s\n", message2);  // example = advent, input = kgzdfaon
    return 0;
}
