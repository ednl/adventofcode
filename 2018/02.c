#include <stdio.h>

#define N 250  // number of IDs (lines) in the input file
#define L ('z' - 'a' + 1)  // letters in the alphabet
#define W 26   // ID width (characters)

static const char *fname = "../aocinput/2018-02-input.txt";
static char id[N][W + 1] = {0};  // +1 for string terminator
static unsigned int bin[L] = {0};

int main(void)
{
    size_t i, j, k;
    FILE *fp = fopen(fname, "r");

    if (fp) {
        // Read all IDs into array
        i = 0;
        while (i < N && fscanf(fp, "%s ", id[i]) > 0) {
            ++i;
        }
        fclose(fp);

        unsigned int c2 = 0, c3 = 0, b2, b3;
        for (i = 0; i < N; ++i) {
            for (j = 0; j < W; ++j) {
                bin[id[i][j] - 'a']++;  // count every different letter in the ID
            }
            b2 = b3 = 0;
            for (j = 0; j < L; ++j) {
                if (bin[j] == 2) {
                    b2 = 1;
                } else if (bin[j] == 3) {
                    b3 = 1;
                }
                bin[j] = 0;  // reset for next ID
            }
            c2 += b2;
            c3 += b3;
        }
        printf("Part 1: %u\n", c2 * c3);

        // Compare every pair of IDs
        for (i = 0; i < N - 1; ++i) {
            for (j = i + 1; j < N; ++j) {
                unsigned int diff = 0;  // count different letters
                for (k = 0; k < W; ++k) {
                    if (id[i][k] != id[j][k]) {
                        if (++diff == 2) {
                            break;  // stop b/c we're looking for exactly 1 difference
                        }
                    }
                }
                if (diff == 1) {  // found them
                    printf("Part 2: ");
                    for (k = 0; k < W; ++k) {
                        if (id[i][k] == id[j][k]) {  // only print identical chars
                            printf("%c", id[i][k]);
                        }
                    }
                    printf("\n");
                    return 0;  // done
                }
            }
        }
    }
    return 0;
}
