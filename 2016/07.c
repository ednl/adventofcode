#include <stdio.h>
#include <stdbool.h>
#include <string.h>  // memset

#define L ('z' - 'a' + 1)

static bool aba[L][L], bab[L][L];

int main(void)
{
    FILE *f = fopen ("../aocinput/2016-07-input.txt", "r");
    if (!f)
        return 1;
    char s[256];
    int tls = 0, ssl = 0;
    while (fgets(s, sizeof s, f)) {
        int i = 0, j = 2;
        bool more = true, outside = true, abba = false, baab = false;
        memset(aba, 0, sizeof aba);
        memset(bab, 0, sizeof bab);
        while (more) {
            switch (s[j]) {
                case '[':
                    i = j + 1;
                    j = i + 2;
                    outside = false;
                    break;
                case ']':
                    i = j + 1;
                    j = i + 2;
                    outside = true;
                    break;
                case '\n':
                case '\0':
                    tls += abba && !baab;
                    for (int a = 0; a < L; ++a)
                        for (int b = 0; b < L; ++b)
                            if (a != b && aba[a][b] && bab[b][a]) {
                                ++ssl;
                                a = L;
                                break;
                            }
                    more = false;
                    break;
                default:
                    if (s[j] == s[j - 2] && s[j] != s[j - 1]) {
                        if (outside)
                            aba[s[j] - 'a'][s[j - 1] - 'a'] = true;
                        else
                            bab[s[j] - 'a'][s[j - 1] - 'a'] = true;
                    }
                    if (j - i == 3) {
                        if (s[i] == s[j] && s[i + 1] == s[j - 1] && s[i] != s[i + 1]) {
                            if (outside)
                                abba = true;
                            else
                                baab = true;
                        }
                        ++i;
                    }
                    ++j;
                    break;
            }
        }
    }
    fclose(f);
    printf("Part 1: %d\n", tls);  // 115
    printf("Part 2: %d\n", ssl);  // 231
    return 0;
}
