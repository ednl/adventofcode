#include <stdio.h>
#include <stdlib.h>  // qsort
#include <unistd.h>  // isatty, fileno
#include <string.h>  // strcmp

#define FNAME "../aocinput/2017-08-input.txt"
#define FSIZE 32768 // needed for my input: 25146

#define LINES 1024
#define NAMES (LINES * 2)

typedef struct reg {
    int hash;
    char name[4];
} Reg;

static char input[FSIZE];
static Reg reg[NAMES];
static int names;

// Hash of 1-3 characters (max = 246)
// Tried different init and shifts until no collisions for my input
static int hash(const char *s)
{
    // int h = 83 ^ *s++;
    // for (int m = 2; *s & 64; m >>= 1) {  // until space which doesn't have the 64 bit
    //     h <<= m;
    //     h ^= *s++;
    // }
    int h = 31 & *s++;
    for (int m = 2; *s & 64; m >>= 1) {  // until space which doesn't have the 64 bit
        h <<= m;
        h ^= *s++;
    }
    return h;
}

static void store(const char *s)
{
    if (names >= NAMES)
        return;
    Reg r = {0};
    r.hash = hash(s);
    for (int i = 0; i < 3 && *s >= 'a'; ++i)
        r.name[i] = *s++;
    reg[names++] = r;
}

static int cmp(const void *p, const void *q)
{
    const Reg *a = p;
    const Reg *b = q;
    if (a->hash < b->hash) return -1;
    if (a->hash > b->hash) return  1;
    return strcmp(a->name, b->name);
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        // Read input file from stdin pipe/redirection
        fread(input, 1, FSIZE, stdin);

    for (const char *c = input; *c; ++c) {  // skip newline, until zero byte
        // Tokenize line into null-separated parameters 0..6
        const char *p[7] = {c};
        for (int i = 1; i < 7; ++i) {
            for (; *c != ' '; ++c);  // skip to space
            p[i] = ++c;  // pointer to first letter
        }
        for (; *c != '\n'; ++c);  // skip to newline
        store(p[0]);
        store(p[4]);
    }

    qsort(reg, names, sizeof *reg, cmp);

    int namecount = 0, hashcount = 0, prevhash = -1, used = 0;
    char prevname[4] = {0};
    int minhash = (1u << 31) - 1;
    int maxhash = 0;
    for (int i = 0; i < names; ++i) {
        if (strcmp(prevname, reg[i].name)) {
            strcpy(prevname, reg[i].name);
            namecount++;
            if (used)  // from previous
                printf("%d)\n", used);
            used = 1;
            printf("%3s %4d %-3s  (", prevhash == reg[i].hash ? ">>>" : "", reg[i].hash, reg[i].name);
        } else
            used++;
        if (prevhash != reg[i].hash) {
            prevhash = reg[i].hash;
            hashcount++;
            if (prevhash < minhash) minhash = prevhash;
            if (prevhash > maxhash) maxhash = prevhash;
        }
    }
    if (used)
        printf("%d)\n", used);
    putchar('\n');
    printf("Register names : %d\n", namecount);
    printf("Hash values    : %d\n", hashcount);
    printf("Min hash value : %d\n", minhash);
    printf("Max hash value : %d\n", maxhash);
}
