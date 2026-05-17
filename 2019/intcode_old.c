///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Intcode VM
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

#include <stdio.h>     // fopen, fclose, fgetc, getdelim, getline, printf
#include <stdlib.h>    // atoi, atol(l), malloc, realloc, free
#include <stdint.h>    // int64_t for Raspberry Pi compatibility where long=32-bit
#include <inttypes.h>  // PRId64 = ld on PC/Mac, lld on RPi
#include <ctype.h>     // isdigit
#include <string.h>    // strcpy, strtok

// On RPi 4, long is 32-bit so to convert to int64_t, use atoll()
#if __SIZEOF_LONG__ == 4
#define atol(X) (atoll(X))
#endif

#define DEBUG

// Virtual machine operation
#define VM_MEMUNIT 4096  // storage space chunk to start with & add when requested
#define VM_RESUME  0
#define VM_RESET   1

// Error codes
#define ERR_OK               0
#define ERR_OPCODE_UNKNOWN   1
#define ERR_OPCODE_UNDEFINED 2
#define ERR_SEGFAULT_PARAM   3
#define ERR_SEGFAULT_READ    4
#define ERR_SEGFAULT_WRITE   5
#define ERR_SEGFAULT_OTHER   6
#define ERR_OUTOFMEMORY      7
#define ERR_ARGUMENTS        8
#define ERR_LIST_EMPTY       9
#define ERR_LIST_NOTFOUND   10

// CPU opcodes
#define CPU_NOP  0  // no operation (extension of spec for zero based array)
#define CPU_ADD  1  // add
#define CPU_MUL  2  // multiply
#define CPU_IN   3  // get input
#define CPU_OUT  4  // send output
#define CPU_JNZ  5  // jump non-zero
#define CPU_JZ   6  // jump zero
#define CPU_LT   7  // compare less than
#define CPU_EQ   8  // compare equal
#define CPU_RBO  9  // relative base offset
#define CPU_HLT 99  // return (halt)

// Opcode parameters
#define PAR_POS 0  // positional
#define PAR_IMM 1  // immediate
#define PAR_OFF 2  // offset
#define PAR_MAX 3  // max number of parameters per opcode

////////// Typedefs & Constants ///////////////////////////////////////////////

// Language entries (instruction definition)
typedef struct Instr {
    int opcode;
    int read, write;
} INSTR;

typedef struct Node NODE, *PNODE;
struct Node {
    int key;
    int64_t val;
    PNODE next;
};

// Language definition: { opcode, read par count, write par count }
// Every read parameter can be positional or immediate
// Write parameters are always positional
static const INSTR cpu[] = {
    { CPU_NOP, 0, 0 },  // 0 extension of spec for zero based array
    { CPU_ADD, 2, 1 },  // 1
    { CPU_MUL, 2, 1 },  // 2
    { CPU_IN , 0, 1 },  // 3
    { CPU_OUT, 1, 0 },  // 4
    { CPU_JNZ, 2, 0 },  // 5
    { CPU_JZ , 2, 0 },  // 6
    { CPU_LT , 2, 1 },  // 7
    { CPU_EQ , 2, 1 },  // 8
    { CPU_RBO, 1, 0 }   // 9
    // CPU_HLT not in array because index not consecutive,
    // so that needs special treatment in vm_exec()
};
static const int cpusize = sizeof cpu / sizeof *cpu;

////////// Globals ////////////////////////////////////////////////////////////

// Virtual machine
static int64_t *vm_cache = NULL, *vm_mem = NULL;
static int vm_cachesize = 0, vm_memsize = 0;

static PNODE list = NULL, listhead = NULL;

////////// Function Declarations //////////////////////////////////////////////

int list_push(int, int64_t);
int list_pop(int *, int64_t *);
int list_find(int, int64_t *);
int list_count(void);
void list_free(void);

int vm_resize(int64_t **, int *, int);
int vm_growto(int64_t **, int *, int);
int vm_growby(int64_t **, int *, int);
int vm_readcsv(const char *);
int vm_refresh(void);
int vm_exec(int);
int64_t vm_input_num(void);
int64_t vm_input_txt(void);
void vm_output_num(int64_t);
void vm_output_txt(int64_t);

////////// Function Definitions ///////////////////////////////////////////////

int list_push(int key, int64_t val)
{
    PNODE n = malloc(sizeof *n);

    if (n == NULL)
        return ERR_OUTOFMEMORY;

    n->key = key;
    n->val = val;
    n->next = NULL;

    if (listhead != NULL)
        listhead->next = n;
    else
        list = n;

    listhead = n;
    return ERR_OK;
}

int list_pop(int *key, int64_t *val)
{
    PNODE n = list;

    if (n == NULL)
        return ERR_LIST_EMPTY;

    *key = n->key;
    *val = n->val;

    if ((list = n->next) == NULL)
        listhead = NULL;

    free(n);
    return ERR_OK;
}

int list_find(int key, int64_t *val)
{
    PNODE n0 = NULL, n1 = list;

    if (n1 == NULL)
        return ERR_LIST_EMPTY;

    while (n1->key != key && n1->next != NULL)
    {
        n0 = n1;
        n1 = n1->next;
    }
    if (n1->key != key)
        return ERR_LIST_NOTFOUND;

    *val = n1->val;

    if (n0 == NULL)  // same as pop
    {
        if ((list = n1->next) == NULL)
            listhead = NULL;
    } else if ((n0->next = n1->next) == NULL)  // last element?
        listhead = n0;

    free(n1);
    return ERR_OK;
}

int list_count(void)
{
    int len = 0;
    PNODE n = list;

    while (n != NULL)
    {
        n = n->next;
        ++len;
    }
    return len;
}

void list_free(void)
{
    PNODE n0 = NULL, n1 = list;

    while (n1 != NULL)
    {
        n0 = n1;
        n1 = n1->next;
        free(n0);
    }
    list = NULL;
    listhead = NULL;
}

int vm_resize(int64_t **mem, int *fromsize, int tosize)
{
    int64_t *p = *mem;
    int n = *fromsize, m = tosize;

    if ((p = realloc(p, m * sizeof *p)) != NULL)
    {
        while (n < m)
            p[n++] = 0;
        *mem = p;
        *fromsize = m;
        return ERR_OK;
    } else
        return ERR_OUTOFMEMORY;  // mem-ptr and size unchanged
}

int vm_growto(int64_t **mem, int *cursize, int newsize)
{
    if (newsize < *cursize || *cursize < 0)
        return ERR_ARGUMENTS;

    if (newsize == *cursize && newsize % VM_MEMUNIT == 0)
        return ERR_OK;  // already aligned

    // Next chunk
    // (realigns cursize if not yet aligned and newsize==cursize)
    int n = (VM_MEMUNIT + newsize - 1) / VM_MEMUNIT * VM_MEMUNIT;

    return vm_resize(mem, cursize, n);
}

int vm_growby(int64_t **mem, int *cursize, int extrasize)
{
    return vm_growto(mem, cursize, *cursize + extrasize);
}

// Read 64-bit integer CSV values from file to cache
// Pre: vm_filename must contain name of readable file
// Ret: number of values read from file & stored in vm_cache
int vm_readcsv(const char *fname)
{
    FILE *fp;        // file pointer
    char *s = NULL;  // dynamically allocated buffer
    size_t t = 0;    // size of buffer
    int n = 0;       // number of values stored
    char *pc;

    // Open single line CVS text file for reading
    if ((fp = fopen(fname, "r")) == NULL)
    {
        #ifdef DEBUG
        printf("Could not open file: %s\n", fname);
        #endif
        return 0;
    }

    // Read the first line which should be the whole file
    if (getline(&s, &t, fp) == 0)
    {
        #ifdef DEBUG
        printf("Could not read from file: %s\n", fname);
        #endif
        goto done;
    }

    pc = strtok(s, ",");
    while (pc != NULL)
    {
        if (n >= vm_cachesize)
            if (vm_growby(&vm_cache, &vm_cachesize, 1) != ERR_OK)
            {
                #ifdef DEBUG
                printf("Out of memory while reading %s at %d\n", fname, n);
                #endif
                goto done;
            }
        vm_cache[n++] = (int64_t)atol(pc);
        pc = strtok(NULL, ",");
    }

done:
    free(s);
    fclose(fp);
    return n;
}

// Copy program from cache to memory, and zero the padding
int vm_refresh(void)
{
    if (vm_cache == NULL || vm_cachesize <= 0)
        return ERR_ARGUMENTS;

    int e;

    if (vm_mem == NULL || vm_memsize < vm_cachesize)
        if ((e = vm_growto(&vm_mem, &vm_memsize, vm_cachesize)) != ERR_OK)
            return e;

    int64_t *src = vm_cache, *dst = vm_mem;
    int n = vm_cachesize, m = vm_memsize - n;

    while (n--)
        *dst++ = *src++;
    while (m--)
        *dst++ = 0;

    return ERR_OK;
}

// Parse and execute all instructions in memory
int vm_exec(int init)
{
    static int ip = 0;    // instruction pointer
    static int base = 0;  // "relative base offset"
    static int tick = 0;
    int instr, opcode, parmode;
    int64_t par[PAR_MAX];
    int i, j, k;

    if (init == VM_RESET || vm_mem == NULL || vm_memsize < vm_cachesize)
    {
        if ((i = vm_refresh()) != ERR_OK)
            return i;
        ip = 0;
        base = 0;
    }

    while (ip >= 0 && ip < vm_memsize)
    {
        // Get opcode and parameter modes from instruction
        instr = vm_mem[ip++];   // get instruction, increment instr pointer
        opcode = instr % 100;   // opcode part of instruction
        if (opcode == CPU_HLT)  // special check for HLT
            return ERR_OK;      //   because not in cpu[]
        instr /= 100;           // this leaves parameter modes

        // Look up opcode, halt if not found or too long
        if (opcode <= 0 || opcode >= cpusize)  // not in the language def?
        {
            #ifdef DEBUG
            printf("Unknown opcode %d at %d\n", opcode, ip - 1);
            #endif
            return ERR_OPCODE_UNKNOWN;
        }
        if (ip + cpu[opcode].read + cpu[opcode].write > vm_memsize)
        {
            #ifdef DEBUG
            printf("Too many parameters for program size at %d\n", ip - 1);
            #endif
            return ERR_SEGFAULT_PARAM;
        }

        k = 0;  // overall parameter count

        // Get "read" parameter(s)
        for (i = 0; i < cpu[opcode].read && k < PAR_MAX; ++i)
        {
            par[k++] = vm_mem[ip++];  // get immediate value, incr par count & instr ptr
            parmode = instr % 10;     // current parameter mode
            instr /= 10;              // prepare next parameter mode
            if (parmode != PAR_IMM)   // par mode is positional or offset?
            {
                if (parmode == PAR_OFF)  // use base offset
                    par[k] += base;
                if (par[k] >= vm_memsize)
                    vm_growto(&vm_mem, &vm_memsize, par[k] + 1);
                if (par[k] < 0 || par[k] >= vm_memsize)
                {
                    #ifdef DEBUG
                    printf("Error: unable to read from %+" PRId64 "\n", par[k]);
                    printf("ip=%d base=%d tick=%d\n", ip - 1, base, tick);
                    #endif
                    return ERR_SEGFAULT_READ;
                }
                par[k] = vm_mem[par[k]];  // get positional value
            }
        }

        // Get "write" parameter (always positional/offset but keep the address this time)
        for (i = 0; i < cpu[opcode].write && k < PAR_MAX; ++i)
        {
            par[k++] = vm_mem[ip++];  // get address param, incr par count & instr ptr
            parmode = instr % 10;     // current parameter mode
            instr /= 10;              // prepare next parameter mode
            if (parmode == PAR_OFF)   // par mode = offset?
                par[k] += base;
            if (par[k] >= vm_memsize)
                vm_growto(&vm_mem, &vm_memsize, par[k] + 1);
            if (par[k] < 0 || par[k] >= vm_memsize)
            {
                #ifdef DEBUG
                printf("Error: unable to write to %+" PRId64 "\n", par[k]);
                printf("ip=%d base=%d tick=%d\n", ip - 1, base, tick);
                #endif
                return ERR_SEGFAULT_WRITE;
            }
        }

        // Advance clock
        ++tick;

        // Execute instruction
        switch (opcode)
        {
            case CPU_NOP: break;
            case CPU_ADD: vm_mem[par[2]] = par[0] + par[1];  break;
            case CPU_MUL: vm_mem[par[2]] = par[0] * par[1];  break;
            case CPU_IN : vm_mem[par[0]] = vm_input_txt();   break;
            case CPU_OUT: vm_output_txt(par[0]);             break;
            case CPU_JNZ: if ( par[0]) ip = par[1];          break;
            case CPU_JZ : if (!par[0]) ip = par[1];          break;
            case CPU_LT : vm_mem[par[2]] = par[0] <  par[1]; break;
            case CPU_EQ : vm_mem[par[2]] = par[0] == par[1]; break;
            case CPU_RBO: base += par[0];                    break;
            case CPU_HLT: return ERR_OK;
            default:
                #ifdef DEBUG
                printf("Internal error: undefined opcode %d\n", opcode);
                #endif
                return ERR_OPCODE_UNDEFINED;
        }
    }
    #ifdef DEBUG
    printf("Internal error: uncaught segfault\n");
    #endif
    return ERR_SEGFAULT_OTHER;
}

int asciichar(char c)
{
    return c >= ' ' || c == '\n';
}

int asciilong(int64_t lc)
{
    return (lc >= ' ' && lc <= 127) || lc == '\n';
}

// Request long int value for input
int64_t vm_input_num(void)
{
    return 0;
}

// Request line of text for input, drip feed into VM per char
int64_t vm_input_txt(void)
{
    static char *s = NULL;
    static size_t t = 0;
    static unsigned int n = 0, i = 0;
    char c = 0;

    while (i < n && !asciichar((c = s[i])))
        ++i;  // skip invalid input (mainly to catch \r on Windows)

    if (i >= n || c == '\n')
    {
        // input exhausted or all invalid
        free(s);  // must free before returning \n or else memory leak
        s = NULL;
        t = n = i = 0;
        if (c == '\n')  // EOL
            return c;
    } else
    {
        // input good
        ++i;
        return c;
    }

    do
    {
        printf("? ");
        n = getline(&s, &t, stdin);
    } while (n <= 1);

    // Check shortcuts
    if (n == 2)
    {
        if (s[0] == 'q')
        {
            free(s);
            printf("Bye.\n");
            exit(0);
        }
        if (t >= 7)  // 5 chars + \n + \0
        {
            switch (s[0])
            {
                case 'n': strcpy(s, "north\n"); break;
                case 's': strcpy(s, "south\n"); break;
                case 'e': strcpy(s, "east\n");  break;
                case 'w': strcpy(s, "west\n");  break;
            }
            n = strlen(s);
        }
    }

    return vm_input_txt();
}

// Process value for output as long int value.
// PRId64 = ld on 64-bit arch, lld on e.g. RPi 4
void vm_output_num(int64_t val)
{
    static int count = 0;
    printf("%d: %" PRId64 "\n", count++, val);
}

// Process value for output as ASCII char (preferred) or long int value (fallback).
void vm_output_txt(int64_t val)
{
    if (asciilong(val))
        printf("%c", (char)val);
    else
        vm_output_num(val);
}

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    if (argc < 2 || !argv || !argv[1])
    {
        printf("Provide file name as command line argument.\n");
        return ERR_ARGUMENTS;
    }

    int64_t inp = argc > 2 && argv[2] ? atol(argv[2]) : 0;

    if (vm_readcsv(argv[1]))
        printf("Error: %d\n", vm_exec(VM_RESET));

    free(vm_mem);
    free(vm_cache);
    return ERR_OK;
}
