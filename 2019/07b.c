///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 7: Amplification Circuit, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

/*
--- Part Two ---

It's no good - in this configuration, the amplifiers can't generate a large
enough output signal to produce the thrust you'll need. The Elves quickly talk
you through rewiring the amplifiers into a feedback loop:

      O-------O  O-------O  O-------O  O-------O  O-------O
0 -+->| Amp A |->| Amp B |->| Amp C |->| Amp D |->| Amp E |-.
   |  O-------O  O-------O  O-------O  O-------O  O-------O |
   |                                                        |
   '--------------------------------------------------------+
                                                            |
                                                            v
                                                     (to thrusters)

Most of the amplifiers are connected as they were before; amplifier A's output
is connected to amplifier B's input, and so on. However, the output from
amplifier E is now connected into amplifier A's input. This creates the
feedback loop: the signal will be sent through the amplifiers many times.

In feedback loop mode, the amplifiers need totally different phase settings:
integers from 5 to 9, again each used exactly once. These settings will cause
the Amplifier Controller Software to repeatedly take input and produce output
many times before halting. Provide each amplifier its phase setting at its
first input instruction; all further input/output instructions are for signals.

Don't restart the Amplifier Controller Software on any amplifier during this
process. Each one should continue receiving and sending signals until it halts.

All signals sent or received in this process will be between pairs of
amplifiers except the very first signal and the very last signal. To start the
process, a 0 signal is sent to amplifier A's input exactly once.

Eventually, the software on the amplifiers will halt after they have processed
the final loop. When this happens, the last output signal from amplifier E is
sent to the thrusters. Your job is to find the largest output signal that can
be sent to the thrusters using the new phase settings and feedback loop
arrangement.

Here are some example programs:

Max thruster signal 139629729 (from phase setting sequence 9,8,7,6,5):
3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5

Max thruster signal 18216 (from phase setting sequence 9,7,8,5,6):
3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10

Try every combination of the new phase settings on the amplifier feedback loop. What is the highest signal that can be sent to the thrusters?
*/

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
#define VM_PAD     0  // storage space to add at end of program
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

// Permutation
static int perm[5 * 120];

////////// Function Declarations //////////////////////////////////////////////

int aoc_thispuzzle(char **);
void aoc_setfilename(int);
int aoc_csvfields(void);
int aoc_csv2cache(void);
void vm_refresh(void);
long vm_exec(int, int, long);
long vm_input(void);
void vm_output(long);

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
	static const char *test = "../aocinput/2019-07-example.txt";
	static const char *fmt = "%02d.txt";

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
long vm_exec(int reset, int phase, long input)
{
	static int ip = 0;    // instruction pointer
	static int base = 0;  // "relative base offset"
	static int tick = 0;  // clock
	int instr, opcode, parmode;
	long par[PAR_MAX];
	int i, j, k, count = 0;

	if (reset)
	{
		vm_refresh();
		ip = 0;
		base = 0;
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
			printf("Unknown opcode %d at %d\n", opcode, ip - 1);
			exit(ERR_OPCODE_UNKNOWN);
			#endif
		}
		if (ip + cpu[i].read + cpu[i].write > vm_memorysize)
		{
			#ifdef DEBUG
			printf("Too many parameters for program size at %d\n", ip - 1);
			exit(ERR_SEGFAULT_PARAM);
			#endif
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
					exit(ERR_SEGFAULT_READ);
					#endif
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
				printf("ip=%d base=%d tick=%d\n", ip, base, tick);
				exit(ERR_SEGFAULT_WRITE);
				#endif
			}
		}

		// Advance clock
		++tick;

		// Execute instruction
		switch (opcode)
		{
			case CPU_ADD: vm_memory[par[2]] = par[0] + par[1];  break;
			case CPU_MUL: vm_memory[par[2]] = par[0] * par[1];  break;
			case CPU_IN : vm_memory[par[0]] = count++ % 2 ? input : phase; break;
			case CPU_OUT: return par[0];                        break;
			case CPU_JNZ: if ( par[0]) ip = par[1];             break;
			case CPU_JZ : if (!par[0]) ip = par[1];             break;
			case CPU_LT : vm_memory[par[2]] = par[0] < par[1];  break;
			case CPU_EQ : vm_memory[par[2]] = par[0] == par[1]; break;
			case CPU_RBO: base += par[0];                       break;
			case CPU_HLT: exit(ERR_OK);
			default :
				#ifdef DEBUG
				printf("Internal error: undefined opcode %d\n", opcode);
				exit(ERR_OPCODE_UNDEFINED);
				#endif
				break;
		}
	}
	exit(ERR_SEGFAULT_OTHER);
}

void makeperm(int k)
{
	static int u[] = {0,0,0,0,0};  // index of used elements
	static int r[] = {0,0,0,0,0};  // permutation result
	static int p = 0;
	int i;

	for (i = 0; i < 5; ++i)
	{
		if (u[i])             // is element i already in use?
			continue;         // if so, skip it
		r[k] = i;             // add unused element to the permutation
		if (k < 4)            // more positions to go?
		{
			u[i] = 1;         // element i is now in use
			makeperm(k + 1);  // go to the next position
			u[i] = 0;         // element i is no longer in use
		} else                // this permutation is ready
		{
			// Do something with the permutation
			// (must be done here because recursive function
			// still only returns once to parent)
			for (i = 0; i < 5; ++i)
				perm[p * 5 + i] = r[i];
			++p;
		}
	}
}

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int i, j, len, ret, inout, max = 0;

	// Init AoC
	srand(time(NULL));
	aoc_setfilename(aoc_thispuzzle(argv));

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
			makeperm(0);
			// Part one
			for (j = 0; j < 120; ++j)
			{
				inout = 0;
				for (i = 0; i < 5; ++i)
					inout = vm_exec(VM_RESET, perm[j * 5 + i], inout);
				if (inout > max)
				{
					max = inout;
					printf("%d: ", j);
					for (i = 0; i < 5; ++i)
						printf("%d", perm[j * 5 + i]);
					printf(": %d\n", max);
				}
			}
			// Part two
			for (j = 0; j < 120; ++j)
			{
				inout = 0;
				for (i = 0; i < 5; ++i)
					inout = vm_exec(VM_RESET, perm[j * 5 + i] + 5, inout);
				if (inout > max)
				{
					max = inout;
					printf("%d: ", j);
					for (i = 0; i < 5; ++i)
						printf("%d", perm[j * 5 + i] + 5);
					printf(": %d\n", max);
				}
			}
		}
		free(vm_memory);
		free(vm_cache);
	}
	return ERR_OK;
}
