#include <stdio.h>
#include <stdlib.h>

static const char *fname = "../aocinput/2018-01-input.txt";

int main(void)
{
    // Part 1
    int *freq = NULL, *modfreq = NULL, shift = 0, dup = 0;
    char *str = NULL;
    size_t N = 0, len = 0, i, j, min = -1;

    FILE *fp = fopen(fname, "r");
    if (fp) {
        // Count lines in input file
        while (getline(&str, &len, fp) > 0) {
            ++N;
        }
        if (N) {
            // Allocate table for cumulative fequencies
            freq = (int *)malloc(N * sizeof *freq);
            modfreq = (int *)malloc(N * sizeof *modfreq);
            if (freq != NULL && modfreq != NULL) {
                // Start with frequency = 0, then calculate from index = 1
                freq[0] = 0;
                i = 1;
                rewind(fp);
                while (getline(&str, &len, fp) > 0 && i < N) {
                    freq[i] = freq[i - 1] + atoi(str);
                    ++i;
                }
                shift = freq[N - 1] + atoi(str);  // value shift at start of new loop
                printf("Part 1: %d\n", shift);

                // Pre-calc of the moduli of the cumulative frequencies
                for (i = 0; i < N; ++i) {
                    modfreq[i] = freq[i] % shift;
                }
                // Check every pair
                // (disregards possibility of duplicate frequency on the very first loop)
                // (also disregards possibility of shift <= 0)
                for (i = 0; i < N - 1; ++i) {
                    for (j = i + 1; j < N; ++j) {
                        if (modfreq[i] == modfreq[j]) {
                            // Number of steps to reach the duplicate frequency
                            // (disregards possibility of f[i] >= f[j], but this never happens for my input)
                            len = (unsigned long)((freq[j] - freq[i]) / shift) * N + i;
                            if (len < min) {
                                min = len;      // lowest number of steps so far
                                dup = freq[j];  // first duplicated frequency (largest of f[i] and f[j])
                            }
                        }
                    }
                }
                printf("Part 2: %d\n", dup);
                free(freq);
                free(modfreq);
            }
        }
        free(str);
        fclose(fp);
    }
    return 0;
}
