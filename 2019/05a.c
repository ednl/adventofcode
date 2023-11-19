///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 5: Sunny with a Chance of Asteroids, part one
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
#define ADD  1  // add
#define MUL  2  // multiply
#define INP  3  // get user input
#define OUT  4  // give output
#define RET 99  // halt program

// Parameters
#define POS  0  // positional
#define IMM  1  // immediate
#define MAXP 3  // max number per opcode

// Read 32-bit integer into buffer
// max 10 digits + 2 optional \r\n + 1 string terminator + 3 safety
#define BUFLEN 16

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Lang {
	int op, ic, oc;  // opcode, input count, output count
} LANG, *PLANG;

// Language definition
const LANG lang[] = {
	{ ADD, 2, 1 },
	{ MUL, 2, 1 },
	{ INP, 0, 1 },
	{ OUT, 1, 0 },
	{ RET, 0, 0 }
};
const int langsize = (sizeof lang) / (sizeof lang[0]);

// Location of program on disk
const char *inp = "../aocinput/2019-05-input.txt";

////////// Functions //////////////////////////////////////////////////////////

// Count values in one-line CSV file
int progsize(void)
{
	FILE *fp;
	char ch;
	int values = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		values = 1;
		while ((ch = fgetc(fp)) != EOF)
			if (ch == ',')
				++values;
		fclose(fp);
	}
	return values;
}

// Read CSV values from file to memory
// Arg: mem must be allocated
int read(int *mem, int len)
{
	FILE *fp;
	char *s = NULL;
	size_t n = 0;
	int i = 0;

	if ((fp = fopen(inp, "r")) != NULL)
	{
		while (i < len && getdelim(&s, &n, ',', fp) > 0)
			mem[i++] = atoi(s);
		free(s);
		fclose(fp);
	}
	return i;
}

// Ask user input
int input(void)
{
	char buf[BUFLEN];

	printf("? ");
	if (fgets(buf, sizeof buf, stdin) != NULL)
		return atoi(buf);
	return 0;
}

// Give output value
void output(int a)
{
	printf("%i\n", a);
}

// Parse and execute instruction in memory mem at address pc
// Ret: next address (-1 = halt)
int exec(int *mem, int pc)
{
	int i, j, op, in, p[MAXP];

	if (pc >= 0 )
	{
		// Parse
		in = mem[pc++];       // get instruction, incr program counter
		op = in % 100;        // opcode part of the instruction
		in /= 100;            // this leaves parameter modes

		for (i = 0; i < langsize; ++i)
			if (lang[i].op == op)
			{
				// Get input parameter(s)
				for (j = 0; j < lang[i].ic; ++j) {
					p[j] = mem[pc++];       // get the immediate value, incr program counter
					if (in % 10 == POS)     // positional?
						p[j] = mem[p[j]];   // get the positional value
					in /= 10;               // next parameter mode
				}

				// Get output parameter
				if (lang[i].oc)
					p[j] = mem[pc++];       // output is always positional, incr program counter

				// Execute
				switch (op) {
					case ADD: mem[p[2]] = p[0] + p[1]; break;
					case MUL: mem[p[2]] = p[0] * p[1]; break;
					case INP: mem[p[0]] = input();     break;
					case OUT: output(p[0]);            break;
					case RET: return -1;    // halt program
				}

				// Done, go to next instruction
				return pc;
			}
	}
	return -1;  // error, halt program
}

// Copy program from src to dst
void copy(int *src, int *dst, int len)
{
	while (len--)
		*dst++ = *src++;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	int *data = NULL, *memory = NULL;
	int len, pc = 0;

	if ((len = progsize()) > 0)
		if ((data = malloc(len * sizeof *data)) != NULL)
		{
			if ((read(data, len)) == len)
				if ((memory = malloc(len * sizeof *memory)) != NULL)
				{
					copy(data, memory, len);
					while ((pc = exec(memory, pc)) >= 0);
					free(memory);
				}
			free(data);
		}
	return 0;
}
