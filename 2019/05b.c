///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 5: Sunny with a Chance of Asteroids, part two
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
#define INP  3  // get input
#define OUT  4  // send output
#define JNZ  5  // jump non-zero
#define JPZ  6  // jump zero
#define CPL  7  // compare less than
#define CPE  8  // compare equal
#define RET 99  // return

// Parameters
#define POS  0  // positional
#define IMM  1  // immediate
#define ARG  3  // max number of parameters per opcode

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
	{ INP, 0, 1 },
	{ OUT, 1, 0 },
	{ JNZ, 2, 0 },
	{ JPZ, 2, 0 },
	{ CPL, 2, 1 },
	{ CPE, 2, 1 },
	{ RET, 0, 0 }
};
const int langsize = (sizeof lang) / (sizeof *lang);

// Location of program on disk
const char *inp = "../aocinput/2019-05-input.txt";

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

// Give output value
void output(int a)
{
	printf("%d\n", a);
}

// Parse and execute all instructions in memory
void exec(int *m, int n)
{
	int i, j, in, op, a[ARG], pc = 0;

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
			return;                  // unknown opcode
		if (pc + lang[i].rc + lang[i].wc > n)
			return;                  // segfault

		// Get read parameter(s)
		for (j = 0; j < lang[i].rc; ++j)
		{
			a[j] = m[pc++];          // get immediate value, increment pc
			if (in % 10 == POS)      // mode = positional parameter?
			{
				if (a[j] < 0 || a[j] >= n)
					return;          // segfault
				a[j] = m[a[j]];      // get positional value
			}
			in /= 10;                // next parameter mode
		}

		// Get output parameter (always positional but keep the address)
		if (lang[i].wc)
		{
			a[j] = m[pc++];  // get address, increment program counter
			if (a[j] < 0 || a[j] >= n)
				return;      // segfault
		}

		// Execute
		switch (op)
		{
			case NOP: break;
			case ADD: m[a[2]] = a[0] + a[1];  break;
			case MUL: m[a[2]] = a[0] * a[1];  break;
			case INP: m[a[0]] = input();      break;
			case OUT: output(a[0]);           break;
			case JNZ: if ( a[0]) pc = a[1];   break;
			case JPZ: if (!a[0]) pc = a[1];   break;
			case CPL: m[a[2]] = a[0] < a[1];  break;
			case CPE: m[a[2]] = a[0] == a[1]; break;
			default: return;  // RET or unknown opcode: halt program
		}
	}
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	int len, *mem = NULL;

	if ((len = size()) > 0)
		if ((mem = malloc(len * sizeof *mem)) != NULL)
		{
			if (read(mem, len) == len)
				exec(mem, len);
			free(mem);
		}
	return 0;
}
