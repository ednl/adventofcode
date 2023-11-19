///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 25: Cryostasis, part one
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
#include <string.h>    // strcpy

#define DEBUG

// Virtual machine operation
#define VM_FNLEN  32  // max file name length of program on disk
#define VM_GROW 1000  // storage space to add when requested
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
static int64_t *vm_cache = NULL, *vm_memory = NULL;
static int vm_cachesize = 0, vm_memorysize = 0;

////////// Function Declarations //////////////////////////////////////////////

int aoc_thispuzzle(char **);
void aoc_setfilename(int);
int aoc_csvfields(void);
int aoc_csv2cache(void);
void vm_refresh(void);
int vm_exec(int);
int64_t vm_input(void);
void vm_output(int64_t);

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
	static const char *test = "../aocinput/2019-25-example.txt";
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

// Read 64-bit integer CSV values from file to cache
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
			vm_cache[i++] = (int64_t)atoll(s);
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
	int64_t *src, *dst;
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
	static int ip = 0;    // instruction pointer
	static int base = 0;  // "relative base offset"
	static int tick = 0;
	int instr, opcode, parmode;
	int64_t par[PAR_MAX], *tmpptr;
	int i, j, k, tmpsize;

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
					printf("Read beyond program size: %+" PRId64 "\n", par[j] - vm_memorysize + 1);
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
			if (par[j] >= vm_memorysize)
			{
				// Try to grow program size in chunks of VM_GROW
				tmpsize = vm_memorysize;
				while (tmpsize < par[j] + 1)
					tmpsize += VM_GROW;
				if ((tmpptr = realloc(vm_memory, tmpsize * sizeof *vm_memory)) != NULL)
				{
					// Success
					#ifdef DEBUG
					printf("Allocated more memory: cur=%d new=%d\n", vm_memorysize, tmpsize);
					printf("ip=%d base=%d tick=%d\n", ip, base, tick);
					#endif
					for (i = vm_memorysize; i < tmpsize; ++i)
						tmpptr[i] = 0;
					vm_memory = tmpptr;
					vm_memorysize = tmpsize;
				} else
				{
					// Out of memory
					#ifdef DEBUG
					printf("Unable to allocate more memory: cur=%d new=%d\n", vm_memorysize, tmpsize);
					printf("ip=%d base=%d tick=%d\n", ip, base, tick);
					#endif
					return ERR_OUTOFMEMORY;
				}
			}
			if (par[j] < 0 || par[j] >= vm_memorysize)
			{
				#ifdef DEBUG
				printf("Write beyond program size: %+" PRId64 "\n", par[j] - vm_memorysize + 1);
				printf("ip=%d base=%d tick=%d\n", ip, base, tick);
				#endif
				return ERR_SEGFAULT_WRITE;
			}
		}

		// Advance clock
		++tick;

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
				printf("Internal error: undefined opcode %d\n", opcode);
				#endif
				return ERR_OPCODE_UNDEFINED;
		}
	}
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

// Request value for input
int64_t vm_input(void)
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

	return vm_input();
}

// Process value for output
void vm_output(int64_t val)
{
	static int count = 0;

	if (asciilong(val))
		printf("%c", (char)val);
	else
		printf("%d: %" PRId64 "\n", count++, val);
}

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int i, d, len, ret;

	// Init AoC
	aoc_setfilename(aoc_thispuzzle(argv));

	// Check program data
	if ((len = aoc_csvfields()) > 0)
	{
		// Allocate memory
		vm_cachesize  = len;
		vm_memorysize = len;  // no initial padding
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
