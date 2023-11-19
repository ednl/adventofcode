#include <stdio.h>
#include <string.h>

#define A ('a')
#define N (16U)
#define M (N - 1U)
#define L (N + 1U)
#define DANCE (1000U * 1000U * 1000U)
#define CACHESIZE (100U)

static char line[L] = {0};

static void spin(unsigned int count)
{
    static char tmp[L] = {0};
    for (unsigned int i = 0; i < N; ++i) {
        tmp[count++ & M] = line[i];
    }
    strcpy((char *)line, (char *)tmp);
}

static void exchange(unsigned int i, unsigned int j)
{
    char t = line[i];
    line[i] = line[j];
    line[j] = t;
}

static void partner(char a, char b)
{
    char todo = 2;
    for (unsigned int i = 0; i < N; ++i) {
        if (line[i] == a) {
            line[i] = b;
            if (!--todo) {
                break;
            }
        } else if (line[i] == b) {
            line[i] = a;
            if (!--todo) {
                break;
            }
        }
    }
}

static void part1(FILE *fp)
{
    int c;
    unsigned int i, j;
    char a, b;
    while ((c = fgetc(fp)) != EOF) {
        if (c == 'x') {
            fscanf(fp, "%u/%u", &i, &j);
            exchange(i, j);
        } else if (c == 's') {
            fscanf(fp, "%u", &i);
            spin(i);
        } else if (c == 'p') {
            fscanf(fp, "%c/%c", &a, &b);
            partner(a, b);
        }
    }
}

int main(void)
{
    char cache[CACHESIZE][L] = {0};
    unsigned int loop = 0;

    FILE *fp = fopen("../aocinput/2017-16-input.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "File not found.");
        return 1;
    }

    // Init
    unsigned int i = 0;
    char c = A;
    while (i < N) {
        line[i++] = c++;
    }
    strcpy((char *)cache[loop++], (char *)line);

    // Part 1
    //   0: abcdefghijklmnop
    //   1: cgpfhdnambekjiol
    part1(fp);
    printf("%s\n", line);

    // Part 2
    //  60: abcdefghijklmnop
    //  1B: gjmiofcnaehpdlbk
    while (loop < CACHESIZE) {
        // Test if original configuration
        i = 0;
        c = A;
        while (line[i++] == c++)
            ;
        if (i == L) {
            // Found the loop
            break;
        } else {
            // Save & next permutation
            strcpy((char *)cache[loop++], (char *)line);
            rewind(fp);
            part1(fp);
        }
    }
    fclose(fp);

    if (loop == CACHESIZE) {
        fprintf(stderr, "Cache too small.");
        return 2;
    }
    printf("%s\n", cache[DANCE % loop]);
    return 0;
}
