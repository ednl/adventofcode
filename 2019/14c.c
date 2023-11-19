#include <stdio.h>
#include <stdlib.h>  // atoi
#include <string.h>  // strtok

#define REACTANTS  8
#define NAMESZ     6
#define EQUATIONS 60

static const char *inp = "../aocinput/2019-14-input.txt";

typedef struct {
    int id, hash, amount;
    char name[NAMESZ];
} Substance;

typedef struct {
    int size;
    Substance product, reactant[REACTANTS];
} Equation;

static Equation equation[EQUATIONS];
static int equations = 0;

// Max 6 chars A-Z followed by \0
static int namehash(const char *s)
{
    int hash = 0;
    while (*s)
        hash = (hash << 5) | (*s++ - 'A');
    return hash;
}

int main(void)
{
    FILE *f = fopen(inp, "r");
    if (!f)
        return 1;

    char *buf = NULL;
    size_t bufsz = 0;
    const char *delim = " ,=>\n";
    while (getline(&buf, &bufsz, f) > 0) {
        Substance sub[9] = {0};
        int subs = 0;
        char *s = strtok(buf, delim);
        while (s) {
            sub[subs].amount = atoi(s);
            if ((s = strtok(NULL, delim))) {
                strcpy(sub[subs].name, s);
                sub[subs].hash = namehash(s);
                s = strtok(NULL, delim);
            }
            ++subs;
        }
        for (int i = 0; i < subs; ++i) {
            printf("%d %s (%d)", sub[i].amount, sub[i].name, sub[i].hash);
            if (i < subs - 2)
                printf(", ");
            else if (i < subs - 1)
                printf(" => ");
            else
                printf("\n");
        }
    }
    fclose(f);
    free(buf);

    return 0;
}
