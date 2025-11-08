///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 14: Space Stoichiometry, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>   // fopen, fgetc, getdelim, printf
#include <stdlib.h>  // atoi, atol

#define DEBUG

#define MAXEQ 100  // maximum number of equations = unique elements
#define MAXELM 10  // maximum number of elements produced by 1 reverse equation
#define MAXNAME 7  // maximum number of letters in element name

////////// Typedefs & Constants ///////////////////////////////////////////////

// Puzzle input
//static const char *inp = "../aocinput/2019-14-example.txt";
static const char *inp = "../aocinput/2019-14-input.txt";

// Element, quantity
typedef struct Elm {
    int ix;  // Index of this element in eq and prod arrays
    int id;  // ID from element name
    long q;  // quantity
} ELM, *PELM;

// Reverse equation
typedef struct RevEq {
    int len; // length of equation = unique elements it produces
    ELM inp; // origin element & quantity
    ELM out[MAXELM]; // elements & quantities produced by this equation
} REVEQ, *PREVEQ;

////////// Globals ////////////////////////////////////////////////////////////

static REVEQ eq[MAXEQ];   // reverse equations
static long prod[MAXEQ];  // quantities produced per element

static int oreid = 0, fuelid = 0;

////////// Function Declarations //////////////////////////////////////////////

int elmindex(int, int);
int elmid(char *);
int elmname(int);
ELM parseelm(char *s);
int readfile(void);
int cmpelm(const void *, const void *);
int cmpeq(const void *, const void *);
void makeindex(int);
void printequations(int);
void printproduction(int);
void init(void);

////////// Function Definitions ///////////////////////////////////////////////

int elmindex(int id, int n)
{
    int i = 0;

    while (i < n && eq[i].inp.id != id)
        ++i;
    return i;
}

int elmid(char *s)
{
    static const int e0 = 'A' - 1;  // A=1, zero=A-1
    static const int en = 'Z';      // Z=26
    static const int base = en - e0 + 1;  // range=0..26=27
    int id = 0;
    char c, *pc;

    pc = s;
    while ((c = *pc++) && c > e0 && c <= en)  // stop at '\0' or out of range
        id = id * base + c - e0;
    return id;
}

int elmname(int id)
{
    static const int e0 = 'A' - 1;  // A=1, zero=A-1
    static const int base = 'Z' - e0 + 1;  // range=0..26=27
    char s[MAXNAME + 1];  // name & '\0'
    int i, n = 0;

    while (id && n < MAXNAME)
    {
        s[n++] = e0 + id % base;
        id /= base;
    }
    s[n] = '\0';
    for (i = n - 1; i >= 0; --i)
        printf("%c", s[i]);
    return n;
}

ELM parseelm(char *s)
{
    int id = 0, q = 0;
    char c, *pc;

    pc = s;
    while ((c = *pc) && (c < '0' || c > '9'))
        pc++;
    if (c)
    {
        q = atoi(pc);
        while ((c = *pc) && (c < 'A' || c > 'Z'))
            pc++;
        if (c)
            id = elmid(pc);
    }
    return (ELM){ 0, id, q };
}

int readfile(void)
{
    FILE *fp;        // file pointer
    char *s = NULL;  // dynamically allocated buffer
    size_t t = 0;    // size of buffer
    char c, *pc;
    int i, j, ix, id, line = 0;

    if ((fp = fopen(inp, "r")) != NULL)
    {
        // Read data
        while (getline(&s, &t, fp) > 0)
        {
            pc = s;  // start at beginning of line
            i = 0;   // element index
            while ((c = *pc) && c != '\r' && c != '\n')
            {
                eq[line].out[i++] = parseelm(pc);
                while ((c = *pc) && c != ',' && c != '>')
                    pc++;
                if (c)
                    pc++;  // skip the divider but not '\0'
                if (c == '>')
                {
                    eq[line].inp = parseelm(pc);
                    break;
                }
            }
            eq[line++].len = i;
        }
        free(s);
        fclose(fp);

    }
    return line;
}

int cmpelm(const void *a, const void *b)
{
    if (((PELM)a)->id < ((PELM)b)->id)
        return -1;
    if (((PELM)a)->id > ((PELM)b)->id)
        return 1;
    return 0;
}

// Pre: global fuelid
int cmpeq(const void *a, const void *b)
{
    if (((PREVEQ)a)->inp.id == ((PREVEQ)b)->inp.id)
        return 0;
    if (((PREVEQ)a)->inp.id == fuelid)
        return -1;
    if (((PREVEQ)b)->inp.id == fuelid)
        return 1;
    if (((PREVEQ)a)->inp.id < ((PREVEQ)b)->inp.id)
        return -1;
    return 1;
}

// Pre: global oreid
void makeindex(int n)
{
    int i, j, ix, id;

    // Sort
    qsort(eq, n, sizeof(REVEQ), cmpeq);
    for (i = 0; i < n; ++i)
        qsort(eq[i].out, eq[i].len, sizeof(ELM), cmpelm);

    // Prepare indices
    for (i = 0; i < n; ++i)
    {
        eq[i].inp.ix = i;
        for (j = 0; j < eq[i].len; ++j)
            eq[i].out[j].ix = elmindex(eq[i].out[j].id, n);
    }

    // Test indices
    for (i = 0; i < n; ++i)
        for (j = 0; j < eq[i].len; ++j)
        {
            ix = eq[i].out[j].ix;
            id = eq[i].out[j].id;
            if (!(ix == n && id == oreid) &&
                id != eq[ix].inp.id)
            {
                printf("Index error\n");
                exit(1);
            }
        }
}

void printequations(int n)
{
    int i, j;

    for (i = 0; i < n; ++i)
    {
        printf("%ld ", eq[i].inp.q);
        elmname(eq[i].inp.id);
        printf("(%d) =>", eq[i].inp.ix);
        for (j = 0; j < eq[i].len; ++j)
        {
            if (j)
                printf(",");
            printf(" %ld ", eq[i].out[j].q);
            elmname(eq[i].out[j].id);
            printf("(%d)", eq[i].out[j].ix);
        }
        printf("\n");
    }
}

void printproduction(int n)
{
    int i, j;

    for (i = 0; i < n; ++i)
        if (prod[i])
        {
            elmname(eq[i].inp.id);
            printf(" %ld\n", prod[i]);
        }
}

void init(void)
{
    int i, j;

    oreid = elmid("ORE");
    fuelid = elmid("FUEL");

    for (i = 0; i < MAXEQ; ++i)
    {
        eq[i].len = MAXELM;
        eq[i].inp = (ELM){ 0, 0, 0 };
        for (j = 0; j < MAXELM; ++j)
            eq[i].out[j] = (ELM){ 0, 0, 0 };
        prod[i] = 0;
    }
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    int i, j, n, done;
    long fuel = 0, ore = 0, target = 1000000000000;

    init();
    if ((n = readfile()))
    {
        makeindex(n);  // also sorts FUEL equation to the top
        //printequations(n);

        fuel = target / 278404;  // 278404 = ORE needed to produce 1 FUEL (= result from part 1)
        prod[0] = --fuel;
        do
        {
            fuel++;
            prod[0]++;  // add 1 FUEL to stock
            do
            {
                done = 1;
                for (i = 0; i < n; ++i)
                    while (prod[i] > 0)
                    {
                        done = 0;
                        prod[i] -= eq[i].inp.q;
                        for (j = 0; j < eq[i].len; ++j)
                            if (eq[i].out[j].id == oreid)
                                ore += eq[i].out[j].q;
                            else
                                prod[eq[i].out[j].ix] += eq[i].out[j].q;
                    }
            } while (!done);
            printf("%ld: %ld\n", fuel, ore);
        } while (ore < target);

        //printproduction(n);
    }

    return 0;
}
