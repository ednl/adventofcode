/**
 * Advent of Code 2023
 * Day 19: Aplenty
 * https://adventofcode.com/2023/day/19
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define NAME "../aocinput/2023-19-example.txt"
#define WFSZ 11
#define WFLEN 4
#define PARTS 5

// Part rating category
typedef enum cat {
    NOCAT=0, X, M, A, S
} Cat;

// Rule comparison
typedef enum cmp {
    NOCMP=0, LT, GT
} Cmp;

// Rule action
typedef enum act {
    REJECT=0, ACCEPT, GONEXT
} Act;

typedef struct rule {
    Cat cat;
    Cmp cmp;
    unsigned val;
    Act act;
    unsigned nextid;
    char nextname[4];
} Rule;

typedef struct workflow {
    Rule rule[WFLEN];
    unsigned id, len;
    char name[4];
} Workflow;

static const char* cat2char = "xmas_";
static const unsigned start = 'i' << 8 | 'n';
static const unsigned accepted = 'A';
static const unsigned rejected = 'R';
static Workflow wf[WFSZ];
static unsigned part[PARTS][4];

static Cat char2cat(const char c)
{
    switch (c) {
        case 'x': return X;
        case 'm': return M;
        case 'a': return A;
        case 's': return S;
    }
    return NOCAT;
}

static void show(void)
{
    for (int i = 0 ; i < WFSZ; ++i) {
        printf("%d: %s{", i, wf[i].name);
        for (int j = 0; j < wf[i].len; ++j) {
            Cat cat = wf[i].rule[j].cat;
            if (cat != NOCAT) {  // conditional
                fputc(cat2char[cat], stdout);
                // if (wf[i].rule[j].lower)
                //     printf(">%d:", wf[i].rule[j].lower);
                // if (wf[i].rule[j].upper)
                //     printf("<%d:", wf[i].rule[j].upper);
            }
            printf("%s", wf[i].rule[j].nextname);
            if (j != wf[i].len - 1)
                printf(",");
        }
        printf("}\n");
    }
    printf("\n");
    for (int i = 0; i < PARTS; ++i)
        printf("{x=%d,m=%d,a=%d,s=%d}\n", part[i][0], part[i][1], part[i][2], part[i][3]);
    printf("\n");
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0 ; i < WFSZ; ++i) {
        char *s, buf[16];
        if (fscanf(f, " %4[^{]{", buf) == 1) {
            strcpy(wf[i].name, buf);
            s = buf;
            while (*s)
                wf[i].id = wf[i].id << 8 | *s++;
        }
        int j = 0;
        for (int c = ','; c == ',' && j < WFLEN && fscanf(f, " %15[^,}]", buf) == 1; ++j, c = fgetc(f)) {
            if ((s = strchr(buf, ':'))) {
                // conditional node
                wf[i].rule[j].cat = char2cat(buf[0]);
                // int* val = buf[1] == '>' ? &wf[i].rule[j].lower : &wf[i].rule[j].upper;
                // s = &buf[2];
                // while (*s != ':')
                //     *val = *val * 10 + (*s++ & 15);
                // ++s;  // to start of condition name
            } else {
                // unconditional node
                wf[i].rule[j].cat = NOCAT;
                s = buf;  // start of buf is start of condition name
            }
            strcpy(wf[i].rule[j].nextname, s);
            while (*s)
                wf[i].rule[j].nextid = wf[i].rule[j].nextid << 8 | *s++;
        }
        wf[i].len = j;
    }
    for (int i = 0; i < PARTS; ++i)
        fscanf(f, " {x=%d,m=%d,a=%d,s=%d}", &part[i][0], &part[i][1], &part[i][2], &part[i][3]);
    fclose(f);
    show();

    for (int i = 0; i < PARTS; ++i) {
        //
    }
    return 0;
}
