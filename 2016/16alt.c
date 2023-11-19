// By https://www.reddit.com/user/askalski
// https://www.reddit.com/r/adventofcode/comments/5ititq/2016_day_16_c_how_to_tame_your_dragon_in_under_a/
// https://gist.github.com/Voltara/f91364d5778fb342bf2443e6fa30b09f

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static uint64_t dragon_parity(uint64_t n);
static uint64_t find_lowest_1(uint64_t n);
static void     solve(uint64_t input_parity, size_t input_len, uint64_t disk_size);

int main(int argc, char **argv)
{
	if (argc != 2 || !*argv[1]) {
		fprintf(stderr, "usage: %s INPUT\n", argv[0]);
		return 0;
	}

	size_t input_len = strlen(argv[1]);
	uint64_t input_parity = 0;

	// Load the input and its reversed complement into a register;
	// the value is shifted left by 1 bit to simplify handling the
	// (remainder == 0) case later on
	for (uint64_t i = 0, j = input_len * 2; i < j; i++, j--) {
		input_parity ^= (uint64_t) (argv[1][i] != '0') << (i + 1); // Forward
		input_parity ^= (uint64_t) (argv[1][i] == '0') << j;       // Reverse complement
	}
	// Compute the parity of all prefixes of the input
	for (uint64_t i = 1; i < 64; i <<= 1) {
		input_parity ^= input_parity << i;
	}

	fputs("Part 1: ", stdout); solve(input_parity, input_len, 272);      putchar('\n');
	fputs("Part 2: ", stdout); solve(input_parity, input_len, 35651584); putchar('\n');

	return 0;
}

void solve(uint64_t input_parity, size_t input_len, uint64_t disk_size)
{
	uint64_t increment = find_lowest_1(disk_size);
	uint64_t previous_parity = 0;

	for (uint64_t length = increment; length <= disk_size; length += increment) {
		// number of dragon bits
		uint64_t dragons = length / (input_len + 1);
		// number of complete cycles (forward and reverse) of the input
		uint64_t input_cycles    = (length - dragons) / (input_len * 2);
		// remainder of input bits
		uint64_t input_remainder = (length - dragons) % (input_len * 2);

		// parity of the dragon bits
		uint64_t p = dragon_parity(dragons);
		// plus parity of all complete input cycles
		p ^= input_cycles & input_len;
		// plus parity of the remainder
		p ^= input_parity >> input_remainder;
		// only need the least significant bit
		p &= 1;

		// checksum digit is the inverted parity bit,
		// XOR with the previous parity calculation
		putchar("10"[p ^ previous_parity]);

		previous_parity = p;
	}
}

// Returns lowest 1-bit in number
uint64_t find_lowest_1(uint64_t n)
{
	return n & -n;
}

// Returns parity of dragon curve of length n
uint64_t dragon_parity(uint64_t n)
{
	uint64_t gray = n ^ (n >> 1);
	return (gray ^ (uint64_t)__builtin_popcountll(n & gray)) & 1;
}
