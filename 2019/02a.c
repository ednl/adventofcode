///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 2: 1202 Program Alarm, part one
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // fopen, fgetc, printf
#include <stdlib.h>  // atoi

#define BUFLEN 16

const char *inp = "../aocinput/2019-02-input.txt";

int main(void)
{
	FILE *fp;
	char val[BUFLEN];
	int *prog;
	int i, j, k, op, pc, c, n = 1;

	if ((fp = fopen(inp, "r")) != NULL)
		while ((c = fgetc(fp)) != EOF)
			if (c == ',')
				++n;

	if (n < 3) {
		puts("File format error.");
		return 1;
	}

	if ((prog = (int *)malloc(n * sizeof(int)))) {

		rewind(fp);
		i = 0;  // digit counter
		pc = 0;  // program counter
		while ((c = fgetc(fp)) != EOF)
			if (c >= '0' && c <= '9' && i < BUFLEN - 1)
				val[i++] = c;
			else {
				if (i > 0 && pc < n) {
					val[i] = '\0';
					prog[pc++] = atoi(val);
				}
				i = 0;
			}
		if (i > 0 && pc < n) {
			val[i] = '\0';
			prog[pc++] = atoi(val);
		}

		if (pc != n) {
			puts("File format error.");
			return 1;
		}

		prog[1] = 12;
		prog[2] = 2;

		pc = 0;
		while (pc < n && (op = prog[pc]) != 99) {
			if (pc + 3 < n) {
				i = prog[pc + 1];
				j = prog[pc + 2];
				k = prog[pc + 3];
				if (i >= 0 && i < n && j >= 0 && j < n && k >= 0 && k < n)
					switch (op) {
						case 1: prog[k] = prog[i] + prog[j]; break;
						case 2: prog[k] = prog[i] * prog[j]; break;
					}
			}
			pc += 4;
		}

		printf("%i\n", prog[0]);
		free(prog);
	}

	return 0;
}
