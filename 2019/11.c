///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 11: Space Police
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
#define PAD 1000  // storage space to add at end of program
#define RESUME 0
#define RESET  1

#define ERR_OK               0
#define ERR_OPCODE_UNKNOWN   1
#define ERR_OPCODE_UNDEFINED 2
#define ERR_SEGFAULT_PARAM   3
#define ERR_SEGFAULT_READ    4
#define ERR_SEGFAULT_WRITE   5

// Opcodes
#define ADD  1L  // add
#define MUL  2L  // multiply
#define IN   3L  // get input
#define OUT  4L  // send output
#define JNZ  5L  // jump non-zero
#define JZ   6L  // jump zero
#define LT   7L  // compare less than
#define EQ   8L  // compare equal
#define RBO  9L  // relative base offset
#define HLT 99L  // return (halt)

// Parameters
#define POS  0L  // positional
#define IMM  1L  // immediate
#define OFF  2L  // offset
#define ARG  3L  // max number of parameters per opcode

#define RADIUS 100

////////// Typedefs & Constants ///////////////////////////////////////////////

// Location of program on disk
static const char *inp = "../aocinput/2019-11-input.txt";

// Language entries
typedef struct Lang {
    long op;
    int rp, wp;
} LANG;

// Language definition: { opcode, read count, write count }
// Every read parameter can be positional or immediate
// Write parameters are always positional
static const LANG lang[] = {
    { ADD, 2, 1 },
    { MUL, 2, 1 },
    { IN , 0, 1 },
    { OUT, 1, 0 },
    { JNZ, 2, 0 },
    { JZ , 2, 0 },
    { LT , 2, 1 },
    { EQ , 2, 1 },
    { RBO, 1, 0 },
    { HLT, 0, 0 }
};
static const int langsize = sizeof lang / sizeof *lang;

////////// Globals ////////////////////////////////////////////////////////////

static long *dat = NULL, *mem = NULL;
static int datsize = 0, memsize = 0;

static int panel[RADIUS * 2 + 1];

////////// Function Declarations //////////////////////////////////////////////

void output(long);
long input(void);
int sizecsv(void);
int readcsv(void);
void copyprog(void);
int execprog(int);
void paint(long col, long dir);

////////// Function Definitions ///////////////////////////////////////////////

// Process value for output
// Arg: val = output value
void output(long val)
{
    printf("%ld\n", val);
}

// Request value for input
// Ret: input value
long input(void)
{
    char *s = NULL;
    size_t t = 0;
    long val = -1;

    printf("? ");
    if (getline(&s, &t, stdin) > 0)
        val = atol(s);
    free(s);
    return val;
}

// Count values in a one-line CSV file by counting commas + 1
int sizecsv(void)
{
    FILE *fp;
    int ch, i = 0;

    if ((fp = fopen(inp, "r")) != NULL)
    {
        i = 1;
        while ((ch = fgetc(fp)) != EOF)
            if (ch == ',')
                ++i;
        fclose(fp);
    }
    return i;
}

// Read long integer CSV values from file to array dat
// Pre: dat must be allocated to size datsize > 0
//      inp must contain name of readable file
// Ret: number of values read
int readcsv(void)
{
    FILE *fp;        // file pointer
    char *s = NULL;  // dynamically allocated buffer
    size_t t = 0;    // size of buffer
    int i = 0;       // values read

    if (dat != NULL && datsize > 0 && (fp = fopen(inp, "r")) != NULL)
    {
        while (i < datsize && getdelim(&s, &t, ',', fp) > 0)
            dat[i++] = atol(s);
        free(s);
        fclose(fp);
    }
    return i;
}

// Copy program from dat to mem and zero the padding
// Pre: dat must be allocated to size datsize > 0
//      mem must be allocated to size memsize >= datsize
void copyprog(void)
{
    long *src, *dst;
    int i;

    if (dat != NULL && mem != NULL && datsize > 0 && datsize <= memsize)
    {
        src = dat;
        dst = mem;
        i = datsize;
        while (i--)
            *dst++ = *src++;
        i = memsize - datsize;
        while (i--)
            *dst++ = 0;
    }
}

// Parse and execute all instructions in memory
int execprog(int reset)
{
    static long base = 0;
    static int pc = 0;

    int i, j, k, retval = ERR_OK;
    long in, op, par[ARG], parmode;

    long vec[2];
    int ivec = 0;

    if (reset)
    {
        copyprog();
        base = 0;
        pc = 0;
    }

    while (pc >= 0 && pc < memsize)
    {
        // Get opcode and parameter modes from instruction
        in = mem[pc++];                // get instruction, increment program counter
        op = in % 100;                 // opcode part of instruction
        in /= 100;                     // this leaves parameter modes

        // Look up opcode, halt if not found or too long
        i = 0;
        while (i < langsize && op != lang[i].op)
            ++i;
        if (i == langsize)             // not in the language def?
        {
            #ifdef DEBUG
            printf("Unknown opcode %ld at %d\n", op, pc - 1);
            #endif
            return ERR_OPCODE_UNKNOWN;
        }
        if (pc + lang[i].rp + lang[i].wp > memsize)
        {
            #ifdef DEBUG
            printf("Too many parameters for program size at %d\n", pc - 1);
            #endif
            return ERR_SEGFAULT_PARAM;
        }

        // Get read parameter(s)
        for (j = 0; j < lang[i].rp; ++j)
        {
            par[j] = mem[pc++];        // get immediate value param, increment program counter
            parmode = in % 10;
            in /= 10;                  // next parameter mode
            if (parmode != IMM)        // mode = positional or offset parameter?
            {
                if (parmode == OFF)
                    par[j] += base;
                if (par[j] < 0 || par[j] >= memsize)
                {
                    #ifdef DEBUG
                    printf("Read beyond program size: %ld\n", par[j] - memsize + 1);
                    #endif
                    return ERR_SEGFAULT_READ;
                }
                par[j] = mem[par[j]];  // get positional value
            }
        }

        // Get write parameter (always positional/offset but keep the address this time)
        if (lang[i].wp)
        {
            par[j] = mem[pc++];        // get address param, increment program counter
            if (in % 10 == OFF)        // mode = offset parameter?
                par[j] += base;
            if (par[j] < 0 || par[j] >= memsize)
            {
                #ifdef DEBUG
                printf("Write beyond program size: %ld\n", par[j] - memsize + 1);
                #endif
                return ERR_SEGFAULT_WRITE;
            }
        }

        // Execute
        switch (op)
        {
            case ADD: mem[par[2]] = par[0] + par[1];  break;
            case MUL: mem[par[2]] = par[0] * par[1];  break;
            case IN : mem[par[0]] = k; break;
            case OUT:
                vec[ivec++] = par[0];
                if (ivec == 3)
                {
                    drawtile(vec[0], vec[1], vec[2]);
                    ivec = 0;
                }
                break;
            case JNZ: if ( par[0]) pc = par[1];       break;
            case JZ : if (!par[0]) pc = par[1];       break;
            case LT : mem[par[2]] = par[0] < par[1];  break;
            case EQ : mem[par[2]] = par[0] == par[1]; break;
            case RBO: base += par[0];                 break;
            case HLT: return retval;
            default :
                #ifdef DEBUG
                printf("Internal error: undefined opcode %ld\n", op);
                #endif
                return ERR_OPCODE_UNDEFINED;
        }
    }
    return retval;
}

// Draw tiles on screen
// Arg: x=-1..41, y=0..23, t=0..4
void paint(long col, long dir)
{
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    int i, len, ret;

    if ((len = sizecsv()) > 0)
    {
        datsize = len;
        memsize = len + PAD;
        dat = malloc(datsize * sizeof *dat);
        mem = malloc(memsize * sizeof *mem);
        if (dat != NULL && mem != NULL && readcsv() == len)
        {
            ret = execprog(RESET);
            printf("ret = %d\n", ret);
        }
        free(mem);
        free(dat);
    }
    return 0;
}
