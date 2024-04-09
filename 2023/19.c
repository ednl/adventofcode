/**
 * Advent of Code 2023
 * Day 19: Aplenty
 * https://adventofcode.com/2023/day/19
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, binsearch
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define EXAMPLE 1

// Input file
#if EXAMPLE == 1
    #define FNAME "../aocinput/2023-19-example.txt"
    #define WORKFLOWS 11
    #define PARTS 5
#else
    #define FNAME "../aocinput/2023-19-input.txt"
    #define WORKFLOWS 541
    #define PARTS 200
#endif

// Max. no. of rules per workflow
#define RULES 4

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
    char nextname[4];
} Rule;

typedef struct workflow {
    char name[4];
    unsigned rules;
    Rule rule[RULES];
} Workflow;

static const char* cat2char = "_xmas";
// static const unsigned start_id = 'i' << 8 | 'n';
// static const unsigned accepted = 'A';
// static const unsigned rejected = 'R';
static Workflow wf[WORKFLOWS];
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
    for (int i = 0; i < WORKFLOWS; ++i) {
        printf("%3d: %-3s { ", i, wf[i].name);
        for (unsigned j = 0; j < wf[i].rules; ++j) {
            Cat cat = wf[i].rule[j].cat;
            if (cat != NOCAT)  // conditional
                printf("%c %c %4u: ", cat2char[cat], wf[i].rule[j].cmp == LT ? '<' : '>', wf[i].rule[j].val);
            switch (wf[i].rule[j].act) {
                case REJECT: fputs("R  ", stdout); break;
                case ACCEPT: fputs("A  ", stdout); break;
                case GONEXT: printf("%-3s", wf[i].rule[j].nextname); break;
            }
            if (j != wf[i].rules - 1)
                printf(", ");
        }
        printf(" }\n");
    }
    printf("\n");
    for (int i = 0; i < PARTS; ++i)
        printf("%3d: { x=%4u, m=%4u, a=%4u, s=%4u }\n", i, part[i][0], part[i][1], part[i][2], part[i][3]);
}

static int cmpname(const void *p, const void *q)
{
    const Workflow * const a = (const Workflow * const)p;
    const Workflow * const b = (const Workflow * const)q;
    if (!strcmp(a->name, "in")) return -1;
    if (!strcmp(b->name, "in")) return  1;
    return strcmp(a->name, b->name);
}

int main(void)
{
    FILE* f = fopen(FNAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < WORKFLOWS; ++i) {
        char *s, buf[16];
        if (fscanf(f, " %4[^{]{", buf) == 1)
            strcpy(wf[i].name, buf);
        unsigned j = 0;
        for (int c = ','; c == ',' && j < RULES && fscanf(f, " %15[^,}]", buf) == 1; ++j, c = fgetc(f)) {
            if ((s = strchr(buf, ':'))) {
                // conditional node
                wf[i].rule[j].cat = char2cat(buf[0]);
                wf[i].rule[j].cmp = buf[1] == '<' ? LT : GT;
                s = &buf[2];
                while (*s != ':')
                    wf[i].rule[j].val = wf[i].rule[j].val * 10 + (*s++ & 15);
                ++s;  // skip ':' to start of action
            } else {
                // unconditional node, .cat=NOCAT, .cmp=NOCMP
                s = buf;  // start of buf is start of action
            }
            switch (*s) {
                case 'R': wf[i].rule[j].act = REJECT; break;
                case 'A': wf[i].rule[j].act = ACCEPT; break;
                default:
                    wf[i].rule[j].act = GONEXT;
                    strcpy(wf[i].rule[j].nextname, s);
            }
        }
        wf[i].rules = j;
    }
    for (int i = 0; i < PARTS; ++i)
        fscanf(f, " {x=%u,m=%u,a=%u,s=%u}", &part[i][0], &part[i][1], &part[i][2], &part[i][3]);
    fclose(f);

    qsort(wf, WORKFLOWS, sizeof *wf, cmpname);
    for (int i = 0; i < WORKFLOWS; ++i) {
        while (wf[i].rules > 1 && wf[i].rule[wf[i].rules - 1].act != GONEXT && wf[i].rule[wf[i].rules - 2].act == wf[i].rule[wf[i].rules - 1].act) {
            wf[i].rule[wf[i].rules - 2] = wf[i].rule[wf[i].rules - 1];
            --wf[i].rules;
        }
        if (wf[i].rules == 1 && wf[i].rule[0].act != GONEXT) {
            // TODO: replace goto with direct reject/accept
        }
    }
    show();

    for (int i = 0; i < PARTS; ++i) {
        //
    }
    return 0;
}
