///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 7: Amplification Circuit, part one
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

#include <stdio.h>   // fopen, fgetc, fgets, printf
#include <stdlib.h>  // atoi

// Opcodes
#define NOP  0  // no operation
#define ADD  1  // add
#define MUL  2  // multiply
#define IN   3  // get input
#define OUT  4  // send output
#define JNZ  5  // jump non-zero
#define JZ   6  // jump zero
#define LT   7  // compare less than
#define EQ   8  // compare equal
#define HLT 99  // return (halt)

// Parameters
#define POS  0  // positional
#define IMM  1  // immediate
#define ARG  3  // max number of parameters per opcode

#define PERMLEN 5

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Lang {
    int op, rc, wc;
} LANG;

// Language definition: { opcode, read count, write count }
// Every read parameter can be positional or immediate
// Write parameters are always positional
const LANG lang[] = {
    { NOP, 0, 0 },
    { ADD, 2, 1 },
    { MUL, 2, 1 },
    { IN , 0, 1 },
    { OUT, 1, 0 },
    { JNZ, 2, 0 },
    { JZ , 2, 0 },
    { LT , 2, 1 },
    { EQ , 2, 1 },
    { HLT, 0, 0 }
};
const int langsize = (sizeof lang) / (sizeof *lang);

// Location of program on disk
const char *inp = "../aocinput/2019-07-input.txt";

////////// Functions //////////////////////////////////////////////////////////

// Count values in a one-line CSV file
int size(void)
{
    FILE *fp;
    char ch;
    int n = 0;

    if ((fp = fopen(inp, "r")) != NULL)
    {
        n = 1;
        while ((ch = fgetc(fp)) != EOF)
            if (ch == ',')
                ++n;
        fclose(fp);
    }
    return n;
}

// Read CSV values from file to memory
// Arg: mem must be allocated
int read(int *m, int n)
{
    FILE *fp;
    char *s = NULL;
    size_t t = 0;
    int p = 0;

    if ((fp = fopen(inp, "r")) != NULL)
    {
        while (p < n && getdelim(&s, &t, ',', fp) > 0)
            m[p++] = atoi(s);
        free(s);
        fclose(fp);
    }
    return p;
}

void copy(int *src, int *dst, int len)
{
    while (len--)
        *dst++ = *src++;
}

// Ask user input
int input(void)
{
    char *s = NULL;
    size_t t = 0;
    int n = 0;

    printf("? ");
    if (getline(&s, &t, stdin) > 0)
        n = atoi(s);
    free(s);
    return n;
}

// Parse and execute all instructions in memory
int exec(int *m, int n, int *uv, int un)
{
    int i, j, in, op, a[ARG], pc = 0, ui = 0, ret = -1;

    while (pc >= 0 && pc < n)
    {
        // Get opcode and modes from instruction
        in = m[pc++];   // get instruction, increment program counter
        op = in % 100;  // opcode part of instruction
        in /= 100;      // this leaves parameter modes

        // Look up opcode, halt if not found or too long
        i = 0;
        while (i < langsize && op != lang[i].op)
            ++i;
        if (i == langsize)           // not in the language def?
            return ret;              // unknown opcode
        if (pc + lang[i].rc + lang[i].wc > n)
            return ret;              // segfault

        // Get read parameter(s)
        for (j = 0; j < lang[i].rc; ++j)
        {
            a[j] = m[pc++];          // get immediate value, increment pc
            if (in % 10 == POS)      // mode = positional parameter?
            {
                if (a[j] < 0 || a[j] >= n)
                    return ret;      // segfault
                a[j] = m[a[j]];      // get positional value
            }
            in /= 10;                // next parameter mode
        }

        // Get output parameter (always positional but keep the address)
        if (lang[i].wc)
        {
            a[j] = m[pc++];  // get address, increment program counter
            if (a[j] < 0 || a[j] >= n)
                return ret;  // segfault
        }

        // Execute
        switch (op)
        {
            case NOP: break;
            case ADD: m[a[2]] = a[0] + a[1];  break;
            case MUL: m[a[2]] = a[0] * a[1];  break;
            case IN : m[a[0]] = ui < un ? uv[ui++] : input(); break;
            case OUT: ret = a[0];             break;
            case JNZ: if ( a[0]) pc = a[1];   break;
            case JZ : if (!a[0]) pc = a[1];   break;
            case LT : m[a[2]] = a[0] < a[1];  break;
            case EQ : m[a[2]] = a[0] == a[1]; break;
            default : return ret;  // RET or unknown opcode: halt program
        }
    }
    return ret;
}

void swap(int *a, int i, int j)
{
    int t;
    if (i >= 0 && j >= 0 && i < PERMLEN && j < PERMLEN)
    {
        t = a[j];
        a[j] = a[i];
        a[i] = t;
    }
}

int series(int *d, int *m, int pn, int input, int *phase, int an, int *max)
{
    int i = 0, inp[2];

    while (i < an)  // every amplifier in series
    {
        copy(d, m, pn);  // fresh copy of the program
        inp[0] = phase[i];
        inp[1] = input;  // previous output becomes next input
        input = exec(m, pn, inp, 2);
    }
    return input;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    int len, *dat = NULL, *mem = NULL;

    int p[PERMLEN] = {0, 1, 2, 3, 4};
    int q[PERMLEN] = {0, 0, 0, 0, 0};
    int inp[2];
    int out, max, i, j;

    if ((len = size()) > 0)
    {
        dat = malloc(len * sizeof *dat);
        mem = malloc(len * sizeof *mem);
        if (dat && mem)
        {
            if (read(dat, len) == len)
            {
                i = max = 0;

                out = 0;  // first input
                for (j = 0; j < PERMLEN; ++j)  // every amplifier in series
                {
                    copy(dat, mem, len);  // fresh copy of the program
                    inp[0] = p[j];
                    inp[1] = out;  // previous output becomes next input
                    out = exec(mem, len, inp, 2);
                }
                if (out > max)
                    max = out;

                while (i < PERMLEN)  // every permutation of phases
                {
                    if (q[i] < i)
                    {
                        if (i % 2)
                            swap(p, q[i], i);
                        else
                            swap(p, 0, i);

                        out = 0;
                        for (j = 0; j < PERMLEN; ++j)  // every amplifier in series
                        {
                            inp[0] = p[j];
                            inp[1] = out;
                            out = exec(mem, len, inp, 2);
                        }
                        if (out > max)
                            max = out;

                        q[i]++;
                        i = 0;
                    } else
                        q[i++] = 0;

                }
                printf("%d\n", max);
            }
        }
        free(mem);
        free(dat);
    }
    return 0;
}
