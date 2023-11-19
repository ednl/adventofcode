///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 9: Sensor Boost
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
#include <stdlib.h>  // atoi, atol
#include <stdint.h>  // int64_t

#define DEBUG

// Opcodes
#define NOP  0L  // no operation
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

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Lang {
	long op;
	int rc, wc;
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
	{ RBO, 1, 0 },
	{ HLT, 0, 0 }
};
const int langsize = (sizeof lang) / (sizeof *lang);

// Location of program on disk
const char *inp = "../aocinput/2019-09-input.txt";

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
int read(long *m, int n)
{
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	int p = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while (p < n && getdelim(&s, &t, ',', fp) > 0)
			m[p++] = atol(s);
		free(s);
		fclose(fp);
	}
	return p;
}

void copy(long *src, long *dst, int len)
{
	while (len--)
		*dst++ = *src++;
}

// Ask user input
long input(void)
{
	char *s = NULL;
	size_t t = 0;
	long n = 0;

	printf("? ");
	if (getline(&s, &t, stdin) > 0)
		n = atol(s);
	free(s);
	return n;
}

// Give output value
void output(long a)
{
	printf("%ld\n", a);
}

// Parse and execute all instructions in memory
long exec(long *m, int n)
{
	int i, j, pc = 0;
	long in, op, a[ARG], mode, base = 0, ret = -1;

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
		{
			#ifdef DEBUG
			printf("Unknown opcode %ld at %d\n", op, pc - 1);
			#endif
			return ret;              // unknown opcode
		}
		if (pc + lang[i].rc + lang[i].wc > n)
		{
			#ifdef DEBUG
			printf("Too many parameter for program size at: %d\n", pc - 1);
			#endif
			return ret;              // segfault
		}

		// Get read parameter(s)
		for (j = 0; j < lang[i].rc; ++j)
		{
			a[j] = m[pc++];          // get immediate value param, increment pc
			mode = in % 10;
			in /= 10;                // next parameter mode
			if (mode != IMM)         // mode = positional or offset parameter?
			{
				if (mode == OFF)
					a[j] += base;
				if (a[j] < 0 || a[j] >= n)
				{
					#ifdef DEBUG
					printf("Read beyond program size: %ld\n", a[j] - n + 1);
					#endif
					return ret;      // segfault
				}
				a[j] = m[a[j]];      // get positional value
			}
		}

		// Get write parameter (always positional/offset but keep the address)
		if (lang[i].wc)
		{
			a[j] = m[pc++];      // get address param, increment program counter
			if (in % 10 == OFF)  // mode = offset parameter?
				a[j] += base;
			if (a[j] < 0 || a[j] >= n)
			{
				#ifdef DEBUG
				printf("Read beyond program size: %ld\n", a[j] - n + 1);
				#endif
				return ret;      // segfault
			}
		}

		// Execute
		switch (op)
		{
			case NOP:                         break;
			case ADD: m[a[2]] = a[0] + a[1];  break;
			case MUL: m[a[2]] = a[0] * a[1];  break;
			case IN : m[a[0]] = input();      break;
			case OUT: output(a[0]);           break;
			case JNZ: if ( a[0]) pc = a[1];   break;
			case JZ : if (!a[0]) pc = a[1];   break;
			case LT : m[a[2]] = a[0] < a[1];  break;
			case EQ : m[a[2]] = a[0] == a[1]; break;
			case RBO: base += a[0];           break;
			case HLT: return ret;             break;
			default :
				#ifdef DEBUG
				printf("Internal error: unknown opcode %ld\n", op);
				#endif
				return ret;
				break;
		}
	}
	return ret;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	long *dat = NULL, *mem = NULL;
	long result = -1;
	int len, extra = 200;

	if ((len = size()) > 0)
	{
		dat = malloc(len * sizeof *dat);
		mem = malloc((len + extra) * sizeof *mem);
		if (dat && mem)
		{
			if (read(dat, len) == len)
			{
				copy(dat, mem, len);
				result = exec(mem, len + extra);
				printf("result=%ld\n", result);
			}
		}
		free(mem);
		free(dat);
	}
	return 0;
}
