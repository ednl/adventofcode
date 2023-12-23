/**
 * Advent of Code 2023
 * Day 19: Aplenty
 * https://adventofcode.com/2023/day/19
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NAME "../aocinput/2023-19-example.txt"
#define WFSZ 11
#define WFLEN 4
#define PARTS 5

typedef enum cat {
    X, M, A, S, NONE
} Cat;

typedef struct cond {
    Cat cat;
    int lower, upper, nextid;
    char nextname[4];
} Cond;

typedef struct workflow {
    Cond cond[WFLEN];
    int id, len;
    char name[4];
} Workflow;

static const char* cat2char = "xmas_";
static const int start = 'i' << 8 | 'n';
static const int accepted = 'A';
static const int rejected = 'R';
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
    return NONE;
}

static void show(void)
{
    for (int i = 0 ; i < WFSZ; ++i) {
        printf("%d: %s{", i, wf[i].name);
        for (int j = 0; j < wf[i].len; ++j) {
            Cat cat = wf[i].cond[j].cat;
            if (cat != NONE) {  // conditional
                fputc(cat2char[cat], stdout);
                if (wf[i].cond[j].lower)
                    printf(">%d:", wf[i].cond[j].lower);
                if (wf[i].cond[j].upper)
                    printf("<%d:", wf[i].cond[j].upper);
            }
            printf("%s", wf[i].cond[j].nextname);
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
                wf[i].cond[j].cat = char2cat(buf[0]);
                int* val = buf[1] == '>' ? &wf[i].cond[j].lower : &wf[i].cond[j].upper;
                s = &buf[2];
                while (*s != ':')
                    *val = *val * 10 + (*s++ & 15);
                ++s;  // to start of condition name
            } else {
                // unconditional node
                wf[i].cond[j].cat = NONE;
                s = buf;  // start of buf is start of condition name
            }
            strcpy(wf[i].cond[j].nextname, s);
            while (*s)
                wf[i].cond[j].nextid = wf[i].cond[j].nextid << 8 | *s++;
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
