#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define N 200  // lines
#define M  14  // digits per line
#define L   7  // segments per digit

static char seg[N][M][L + 1] = {0};
static size_t len[N][M] = {0};  // avoid calling strlen every time

// Does A contain B? (unsorted so has to check the whole range every time)
static bool contains(const char *a, const char *b)
{
    while (b && *b) {           // while b has chars
        const char *c = a;      // a is needed to restart every loop so don't use it to walk through a[]
        while (*c && *c != *b)  // find matching char or end up at \0 terminator
            c++;
        if (!*c)                // if char from b not found in a
            return false;
        b++;                    // next char from b
    }
    return true;                // all chars from b were found in a
}

int main(void)
{
    int ans = 0;
    FILE *f = fopen("../aocinput/2021-08-input.txt", "r");
    // Read N lines of M strings
    for (int i = 0, j = 0; i < N && fscanf(f, "%7s ", seg[i][j]); ++j == M && ++i && (j = 0)) {
        size_t k = len[i][j] = strlen(seg[i][j]);
        if (j == 9)
            fgetc(f);  // discard '|'
        if (j >= 10 && ((k >= 2 && k <= 4) || k == 7))
            ++ans;
    }
    fclose(f);
    printf("Part 1: %d\n", ans);  // 534

    ans = 0;
    for (int i = 0; i < N; ++i) {
        // Segment patterns 1, 4, 9 used as deciders
        char pat1[L + 1], pat4[L + 1], pat9[L + 1];

        // Find three deciders in first 10 digits
        // (two loops because to find 9, 4 is needed first)
        for (int j = 0; j < 10; ++j)
            switch (len[i][j]) {
                case 2: strcpy(pat1, seg[i][j]); break;       // 1 is needed later to find 3 and 6
                case 4: strcpy(pat4, seg[i][j]); break;       // 4 is needed later to find 9
            }
        for (int j = 0; j < 10; ++j)
            if (len[i][j] == 6 && contains(seg[i][j], pat4))  // use 4 to find 9
                strcpy(pat9, seg[i][j]);                      // 9 is needed later to find 5

        // len 2: "1" = ..c..f.
        // len 3: "7" = a.c..f.
        // len 4: "4" = .bcd.f.
        // len 7: "8" = abcdefg
        // len 5: "3" = a.cd.fg  step 1: 3 contains 1
        //        "5" = ab.d.fg  step 2: 9 contains 5
        //        "2" = a.cde.g  step 3: 2 is left
        // len 6: "6" = ab.defg  step 1: 6 does not contain 1
        //        "9" = abcd.fg  step 2: 9 contains 4
        //        "0" = abc.efg  step 3: 0 is left

        // Determine value of last 4 digits
        int val = 0;
        for (int j = 10; j < M; ++j) {
            val *= 10;
            switch (len[i][j]) {
                case 2: val += 1; break;
                case 3: val += 7; break;
                case 4: val += 4; break;
                case 5:
                    if (contains(seg[i][j], pat1)) {
                        val += 3;
                    } else if (contains(pat9, seg[i][j])) {
                        val += 5;
                    } else {
                        val += 2;
                    }
                    break;
                case 6:
                    if (!contains(seg[i][j], pat1)) {
                        val += 6;
                    } else if (contains(seg[i][j], pat4)) {
                        val += 9;
                    } else {
                        val += 0;
                    }
                    break;
                case 7: val += 8; break;
            }
        }
        ans += val;
    }
    printf("Part 2: %d\n", ans);  // 1070188
    return 0;
}
