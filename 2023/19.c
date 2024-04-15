/**
 * Advent of Code 2023
 * Day 19: Aplenty
 * https://adventofcode.com/2023/day/19
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, bsearch
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define EXAMPLE 1

// Input file
#if EXAMPLE == 1
    #define FNAME "../aocinput/2023-19-example.txt"
    #define WORKFLOWS 16
    #define PARTS 8
#else
    #define FNAME "../aocinput/2023-19-input.txt"
    #define WORKFLOWS 640
    #define PARTS 256
#endif

// Max. no. of rules per workflow
#define RULES 8

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
    unsigned rulecount;
    Rule rule[RULES];
} Workflow;

static const char* cat2char = "_xmas";
static Workflow wf[WORKFLOWS];
static unsigned part[PARTS][4];
static unsigned wfcount, partcount;

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

static bool parse(unsigned * const workflows, unsigned * const parts)
{
    FILE* f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return false; }

    char *s, buf[16];
    const Workflow * const w_end = wf + WORKFLOWS;
    for (Workflow *w = wf; w != w_end && fscanf(f, " %4[^{]{", buf) == 1; ++w, ++(*workflows)) {
        strcpy(w->name, buf);
        int c = ',';
        const Rule * const r_end = w->rule + RULES;
        for (Rule *r = w->rule; c == ',' && r != r_end && fscanf(f, "%15[^,}]", buf) == 1; ++r, ++(w->rulecount), c = fgetc(f)) {
            if ((s = strchr(buf, ':'))) {
                // conditional node
                r->cat = char2cat(buf[0]);
                r->cmp = buf[1] == '<' ? LT : GT;
                s = &buf[2];
                while (*s != ':')
                    r->val = r->val * 10 + (*s++ & 15);
                ++s;  // skip ':' to start of action
            } else {
                // unconditional node, .cat=NOCAT, .cmp=NOCMP
                s = buf;  // start of buf is start of action
            }
            switch (*s) {
                case 'R': r->act = REJECT; break;
                case 'A': r->act = ACCEPT; break;
                default:
                    r->act = GONEXT;
                    strcpy(r->nextname, s);
            }
        }
    }

    for (int i = 0; i < PARTS && fscanf(f, " {x=%u,m=%u,a=%u,s=%u}", &part[i][0], &part[i][1], &part[i][2], &part[i][3]) == 4; *parts = ++i)
        ;

    fclose(f);
    return true;
}

static unsigned findrule(const char * const name)
{
    for (unsigned i = 0; i < WORKFLOWS; ++i)
        for (unsigned j = 0; j < wf[i].rulecount; ++j)
            if (wf[i].rule[j].act == GONEXT && !strcmp(wf[i].rule[j].nextname, name)) {
                return i << 16 | j;
            }
    return -1;
}

static void show(void)
{
    for (int i = 0; i < WORKFLOWS; ++i) {
        printf("%3d: %-3s { ", i, wf[i].name);
        for (unsigned j = 0; j < wf[i].rulecount; ++j) {
            Cat cat = wf[i].rule[j].cat;
            if (cat != NOCAT)  // conditional
                printf("%c %c %4u: ", cat2char[cat], wf[i].rule[j].cmp == LT ? '<' : '>', wf[i].rule[j].val);
            switch (wf[i].rule[j].act) {
                case REJECT: fputs("R  ", stdout); break;
                case ACCEPT: fputs("A  ", stdout); break;
                case GONEXT: printf("%-3s", wf[i].rule[j].nextname); break;
            }
            if (j != wf[i].rulecount - 1)
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
    if (!parse(&wfcount, &partcount))
        return 1;

    qsort(wf, wfcount, sizeof *wf, cmpname);
    for (int i = 0; i < wfcount; ++i) {
        unsigned rc = wf[i].rulecount;
        while (rc > 1 && wf[i].rule[rc - 1].act != GONEXT && wf[i].rule[rc - 1].act == wf[i].rule[rc - 2].act) {
            wf[i].rule[rc - 2] = wf[i].rule[rc - 1];
            --rc;
        }
        if (rc == 1 && wf[i].rule[0].act != GONEXT) {
            // TODO: replace goto with direct reject/accept
        }
        wf[i].rulecount = rc;
    }
    show();

    for (int i = 0; i < partcount; ++i) {
        //
    }
    return 0;
}
