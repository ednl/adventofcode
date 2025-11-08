///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 15: Oxygen System, part one
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
#include <ctype.h>   // isdigit
#include <time.h>    // time for srand
#include <string.h>  // strcpy

#define DEBUG

// Virtual machine operation
#define VM_FNLEN  32  // max file name length of program on disk
#define VM_PAD  1000  // storage space to add at end of program
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

// CPU opcodes
#define CPU_ADD  1L  // add
#define CPU_MUL  2L  // multiply
#define CPU_IN   3L  // get input
#define CPU_OUT  4L  // send output
#define CPU_JNZ  5L  // jump non-zero
#define CPU_JZ   6L  // jump zero
#define CPU_LT   7L  // compare less than
#define CPU_EQ   8L  // compare equal
#define CPU_RBO  9L  // relative base offset
#define CPU_HLT 99L  // return (halt)

// Opcode parameters
#define PAR_POS 0L  // positional
#define PAR_IMM 1L  // immediate
#define PAR_OFF 2L  // offset
#define PAR_MAX 3L  // max number of parameters per opcode

// Game
#define MAZE_NODIR  0
#define MAZE_NORTH  1
#define MAZE_SOUTH  2
#define MAZE_WEST   3
#define MAZE_EAST   4
#define MAZE_VOID  -1
#define MAZE_WALL   0
#define MAZE_FREE   1
#define MAZE_DEST   2
#define MAZE_DIMX  41
#define MAZE_DIMY  40
#define MAZE_X0    21
#define MAZE_Y0    20

////////// Typedefs & Constants ///////////////////////////////////////////////

// Language entries (instruction definition)
typedef struct Instr {
    int opcode;
    int read, write;
} INSTR;

// Language definition: { opcode, read par count, write par count }
// Every read parameter can be positional or immediate
// Write parameters are always positional
static const INSTR cpu[] = {
    { CPU_ADD, 2, 1 },
    { CPU_MUL, 2, 1 },
    { CPU_IN , 0, 1 },
    { CPU_OUT, 1, 0 },
    { CPU_JNZ, 2, 0 },
    { CPU_JZ , 2, 0 },
    { CPU_LT , 2, 1 },
    { CPU_EQ , 2, 1 },
    { CPU_RBO, 1, 0 },
    { CPU_HLT, 0, 0 }
};
static const int cpusize = sizeof cpu / sizeof *cpu;

////////// Globals ////////////////////////////////////////////////////////////

// Virtual machine
static char vm_filename[VM_FNLEN + 1];
static long *vm_cache = NULL, *vm_memory = NULL;
static int vm_cachesize = 0, vm_memorysize = 0;

// Puzzle 15
static int orientation = 0, steps = 0;
static int droidx = MAZE_X0, droidy = MAZE_Y0;
static int destx = -1, desty = -1;
static int maze[MAZE_DIMX * MAZE_DIMY];

////////// Function Declarations //////////////////////////////////////////////

int aoc_thispuzzle(char **);
void aoc_setfilename(int);
int aoc_csvfields(void);
int aoc_csv2cache(void);
void vm_refresh(void);
int vm_exec(int);
long vm_input(void);
void vm_output(long);

// Maze functions
int maze_index(int, int);
int maze_peek(int, int, int);
int maze_deadend(int, int);
int maze_reverse(int);
void maze_draw(void);
/*
void makemove(int);
*/

////////// Function Definitions ///////////////////////////////////////////////

int aoc_thispuzzle(char **exe)
{
    if (!exe || !*exe)
        return 0;

    char *pc;
    int c;

    pc = *exe;
    while ((c = *pc) && !isdigit(c))
        ++pc;
    return atoi(pc);
}

void aoc_setfilename(int num)
{
    static const char *test = "../aocinput/2019-15-example.txt";
    static const char *fmt = "../aocinput/2019-%02d-input.txt";

    if (num)
        snprintf(vm_filename, VM_FNLEN, fmt, num);
    else
        strcpy(vm_filename, test);
}

// Count values in a one-line CSV file by counting commas + 1
int aoc_csvfields(void)
{
    FILE *fp;
    int c, i = 0;

    if ((fp = fopen(vm_filename, "r")) != NULL)
    {
        i = 1;
        while ((c = fgetc(fp)) != EOF)
            if (c == ',')
                ++i;
        fclose(fp);
    }
    return i;
}

// Read long integer CSV values from file to cache
// Pre: vm_cache must be allocated to size vm_cachesize > 0
//      vm_filename must contain name of readable file
// Ret: number of values read
int aoc_csv2cache(void)
{
    FILE *fp;        // file pointer
    char *s = NULL;  // dynamically allocated buffer
    size_t t = 0;    // size of buffer
    int i = 0;       // values read

    if (vm_cache != NULL && vm_cachesize > 0 &&
        (fp = fopen(vm_filename, "r")) != NULL)
    {
        while (i < vm_cachesize && getdelim(&s, &t, ',', fp) > 0)
            vm_cache[i++] = atol(s);
        free(s);
        fclose(fp);
    }
    return i;
}

// Copy program from cache to memory, and zero the padding
// Pre: dat must be allocated to size datsize > 0
//      mem must be allocated to size memsize >= datsize
void vm_refresh(void)
{
    long *src, *dst;
    int i;

    if (vm_cache != NULL && vm_memory != NULL &&
        vm_cachesize > 0 && vm_cachesize <= vm_memorysize)
    {
        src = vm_cache;
        dst = vm_memory;
        i = vm_cachesize;
        while (i--)
            *dst++ = *src++;
        i = vm_memorysize - vm_cachesize;
        while (i--)
            *dst++ = 0;
    }
}

// Parse and execute all instructions in memory
int vm_exec(int reset)
{
    static int base = 0;  // "relative base offset"
    static int ip = 0;    // instruction pointer
    int instr, opcode, parmode;
    long par[PAR_MAX];
    int i, j, k;

    if (reset)
    {
        vm_refresh();
        base = 0;
        ip = 0;
    }

    while (ip >= 0 && ip < vm_memorysize)
    {
        // Get opcode and parameter modes from instruction
        instr = vm_memory[ip++];       // get instruction, increment instr pointer
        opcode = instr % 100;          // opcode part of instruction
        instr /= 100;                  // this leaves parameter modes

        // Look up opcode, halt if not found or too long
        i = 0;
        while (i < cpusize && opcode != cpu[i].opcode)
            ++i;
        if (i == cpusize)             // not in the language def?
        {
            #ifdef DEBUG
            printf("Unknown opcode %ld at %d\n", opcode, ip - 1);
            #endif
            return ERR_OPCODE_UNKNOWN;
        }
        if (ip + cpu[i].read + cpu[i].write > vm_memorysize)
        {
            #ifdef DEBUG
            printf("Too many parameters for program size at %d\n", ip - 1);
            #endif
            return ERR_SEGFAULT_PARAM;
        }

        // Get "read" parameter(s)
        for (j = 0; j < cpu[i].read; ++j)
        {
            par[j] = vm_memory[ip++];  // get immediate value, increment instr pointer
            parmode = instr % 10;
            instr /= 10;               // next parameter mode
            if (parmode != PAR_IMM)    // par mode = positional or offset?
            {
                if (parmode == PAR_OFF)  // use base offset
                    par[j] += base;
                if (par[j] < 0 || par[j] >= vm_memorysize)
                {
                    #ifdef DEBUG
                    printf("Read beyond program size: %ld\n", par[j] - vm_memorysize + 1);
                    #endif
                    return ERR_SEGFAULT_READ;
                }
                par[j] = vm_memory[par[j]];  // get positional value
            }
        }

        // Get "write" parameter (always positional/offset but keep the address this time)
        if (cpu[i].write)
        {
            par[j] = vm_memory[ip++];   // get address param, increment program counter
            if (instr % 10 == PAR_OFF)  // par mode = offset?
                par[j] += base;
            if (par[j] < 0 || par[j] >= vm_memorysize)
            {
                #ifdef DEBUG
                printf("Write beyond program size: %ld\n", par[j] - vm_memorysize + 1);
                #endif
                return ERR_SEGFAULT_WRITE;
            }
        }

        // Execute instruction
        switch (opcode)
        {
            case CPU_ADD: vm_memory[par[2]] = par[0] + par[1];  break;
            case CPU_MUL: vm_memory[par[2]] = par[0] * par[1];  break;
            case CPU_IN : vm_memory[par[0]] = vm_input();       break;
            case CPU_OUT: vm_output(par[0]);                    break;
            case CPU_JNZ: if ( par[0]) ip = par[1];             break;
            case CPU_JZ : if (!par[0]) ip = par[1];             break;
            case CPU_LT : vm_memory[par[2]] = par[0] < par[1];  break;
            case CPU_EQ : vm_memory[par[2]] = par[0] == par[1]; break;
            case CPU_RBO: base += par[0];                       break;
            case CPU_HLT: return ERR_OK;
            default :
                #ifdef DEBUG
                printf("Internal error: undefined opcode %ld\n", opcode);
                #endif
                return ERR_OPCODE_UNDEFINED;
        }
    }
    return ERR_SEGFAULT_OTHER;
}

// Request value for input
long vm_input(void)
{
    /*
    static int count = 0;
    char *s = NULL;
    size_t t = 0;
    long val = -1;

    printf("%d ? ", count);
    if (getline(&s, &t, stdin) > 0)
        val = atol(s);
    free(s);
    count++;
    return val;
    */
    return orientation;
}

// Process value for output
void vm_output(long val)
{
    /*
    static int count = 0;

    printf("%d : %ld\n", count++, val);
    */
}

int maze_index(int x, int y)
{
    return y * MAZE_DIMX + x;
}

int maze_peek(int x, int y, int dir)
{
    int i = -1;  // illegal index

    switch (dir)
    {
        case MAZE_NORTH: i = maze_index(x, y - 1); break;
        case MAZE_SOUTH: i = maze_index(x, y + 1); break;
        case MAZE_WEST : i = maze_index(x - 1, y); break;
        case MAZE_EAST : i = maze_index(x + 1, y); break;
    }
    if (i >= 0 && i < MAZE_DIMX * MAZE_DIMY)  // legal index?
        return maze[i];
    return MAZE_WALL;
}

int maze_deadend(int x, int y)
{
    int dir, wallcount = 0;

    for (dir = MAZE_NORTH; dir <= MAZE_EAST; ++dir)
        if (maze_peek(x, y, dir) == MAZE_WALL)
            ++wallcount;
    return wallcount >= 3;
}

int maze_reverse(int dir)
{
    int rev = MAZE_NODIR;

    switch (dir)
    {
        case MAZE_NORTH: rev = MAZE_SOUTH; break;
        case MAZE_SOUTH: rev = MAZE_NORTH; break;
        case MAZE_WEST : rev = MAZE_EAST;  break;
        case MAZE_EAST : rev = MAZE_WEST;  break;
    }
    return rev;
}

// Draw maze
void maze_draw(void)
{
    int x, y;

    //printf("\033[2J");  // clear screen
    for (x = 0; x < MAZE_DIMX; ++x)
        printf("-");
    printf("--\n");

    for (y = 0; y < MAZE_DIMY; ++y)
    {
        printf("|");
        for (x = 0; x < MAZE_DIMX; ++x)
        {
            if (x == droidx && y == droidy)
                printf("o");
            else if (x == MAZE_X0 && y == MAZE_Y0)
                printf("+");
            else
                switch (maze[maze_index(x, y)])
                {
                    case MAZE_VOID: printf(" "); break;
                    case MAZE_WALL: printf("#"); break;
                    case MAZE_FREE: printf("."); break;
                    case MAZE_DEST: printf("X"); break;
                }
        }
        printf("|\n");
    }

    for (x = 0; x < MAZE_DIMX; ++x)
        printf("-");
    printf("--\n");
}

/*
void makemove(int resp)
{
    int i, m, x = droidx, y = droidy;
    int todo[4], todocount = 0;
    int been[4], beencount = 0;

    switch (move)
    {
        case NORTH: y--; break;
        case SOUTH: y++; break;
        case WEST : x--; break;
        case EAST : x++; break;
    }

    i = getindex(x, y);
    if (i < 0 || i >= XDIM * YDIM || x < 0 || x >= XDIM || y < 0 || y >= YDIM)
    {
        #ifdef DEBUG
        drawmaze();
        printf("Illegal move to %d,%d (resp=%d)\n", x, y, resp);
        #endif
        exit(1);
    }

    maze[i] = resp;
    if (resp != WALL)
    {
        xbot = x;
        ybot = y;
        ++steps;
    }
    if (resp == TARGET)
    {
        //drawmaze();
        //printf("Found target at %d,%d\n", x, y);
        //printf("%d steps from %d,%d\n", steps, X0, Y0);
        drawpartial(X0, Y0, x, y);
        steps = abs(x - X0) + abs(y - Y0);
        printf("min steps: %d\n", steps);
        exit(0);
    }
    for (m = NORTH; m <= EAST; ++m)
    {
        switch (peek(m))
        {
            case TODO  : todo[todocount++] = m; break;
            case EMPTY :
            case TARGET: been[beencount++] = m; break;
        }
    }
    if (todocount == 1)
        m = todo[0];
    else if (todocount > 1)
        m = todo[rand() / ((RAND_MAX + 1U) / todocount)];
    else if (beencount == 1)
        m = been[0];
    else if (beencount > 1)
    {
        r = reverse(move);
        skip = 4;
        for (i = 0; i < beencount; ++i)
            if (been[i] == r)
            {
                skip = i;
                beencount--;
                break;
            }
        i = beencount == 1 ? 0 : rand() / ((RAND_MAX + 1U) / beencount);
        if (i >= skip)
            ++i;
        m = been[i];
    } else
    {
        #ifdef DEBUG
        drawmaze();
        printf("No legal moves from %d,%d\n", xbot, ybot);
        #endif
        exit(1);
    }
    move = m;
}
*/

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int i, len, ret;

    // Init AoC
    srand(time(NULL));
    aoc_setfilename(thispuzzle(argv));

    // Init maze
    for (i = 0; i < MAZE_DIMX * MAZE_DIMY; ++i)
        maze[i] = MAZE_VOID;

    // Check program data
    if ((len = aoc_csvfields()) > 0)
    {
        // Allocate memory
        vm_cachesize  = len;
        vm_memorysize = len + VM_PAD;
        vm_cache  = malloc(vm_cachesize  * sizeof *vm_cache );
        vm_memory = malloc(vm_memorysize * sizeof *vm_memory);
        // Read into memory
        if (vm_cache != NULL && vm_memory != NULL && aoc_csv2cache() == len)
        {
            // Run program
            if ((ret = vm_exec(VM_RESET)) != ERR_OK)
                printf("Error: %d\n", ret);
        }
        free(vm_memory);
        free(vm_cache);
    }
    return ERR_OK;
}
