/**
 * By Nat Schaeffer
 * via https://pouet.chapril.org/@natchouf/115662180051130781
 * from https://codeberg.org/natchouf/advent_of_code_2025/src/branch/master/solve4.c
 *
 * Compile:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04_natchouf.c
 *    with startstoptimer.c from https://github.com/ednl/adventofcode/blob/main/startstoptimer.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements by ednl:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  875 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 3157 µs
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Timer library by ednl
// from https://github.com/ednl/adventofcode/blob/main/startstoptimer.h
#include "../startstoptimer.h"

char* read_text_matrix(const char* fname, long *nx, long *ny)
{
	FILE* f = fopen(fname,"r");
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	rewind(f);

	//printf("file_size = %ld\n", file_size);
	char* mx = (char*) malloc(file_size+1);
	fgets(mx, file_size+1, f);
	const long nc = strlen(mx) - 1;		// forget about new-line character
	long nl = 1;
	while(!feof(f)) {
		if (fgets(mx + nl*nc, nc+3, f))  nl++;
	}
	*nx = nc;	*ny = nl;
	return mx;
}

int main(void) {
	long nx, ny;
	char* data = read_text_matrix("../aocinput/2025-04-input.txt", &nx, &ny);

	// rearrange data:
	char* data2 = malloc((nx+2)*(ny+2));
	for (int i=0; i<ny; i++) {
		data2[(i+1)*(nx+2)] = 0;
		for (int j=0; j<nx; j++) {
			data2[(i+1)*(nx+2) + j+1] = data[i*nx + j] - '.';		// translate the '.' into 0 values.
		}
		data2[(i+1)*(nx+2) + nx+1] = 0;
	}
	memset(data2, 0, nx+2);
	memset(data2+(ny+1)*(nx+2), 0, nx+2);

	// Start timer after reading from disk and parsing
	starttimer();

	int accessible = 0;
	for (int i=1; i<=ny; i++) {
		for (int j=1; j<=nx; j++) {
			long ofs = i*(nx+2) + j;
			//printf("%d",data2[ofs]/('@'-'.'));
			int neighboors = data2[ofs + 1] + data2[ofs - 1] + data2[ofs + (nx+2)] + data2[ofs - (nx+2)]
							+ data2[ofs + nx+1] + data2[ofs + nx+3] + data2[ofs - (nx+3)] + data2[ofs - (nx+1)];
			if (neighboors < 4*('@'-'.') && data2[ofs]!=0) accessible ++;
		}
		//printf("\n");
	}

	printf("part 1 answer : %d\n", accessible);

	int removed_overall = 0;
	while(1) {
		int removed = 0;
		for (int i=1; i<=ny; i++) {
			for (int j=1; j<=nx; j++) {
				long ofs = i*(nx+2) + j;
				//printf("%d",data2[ofs]/('@'-'.'));
				int neighboors = data2[ofs + 1] + data2[ofs - 1] + data2[ofs + (nx+2)] + data2[ofs - (nx+2)]
								+ data2[ofs + nx+1] + data2[ofs + nx+3] + data2[ofs - (nx+3)] + data2[ofs - (nx+1)];
				if (neighboors < 4*('@'-'.') && data2[ofs]!=0) {	removed++;	data2[ofs]=0;	}
			}
			//printf("\n");
		}
		removed_overall += removed;
		if (removed==0) break;
	}

	printf("part 2 answer : %d\n", removed_overall);

	printf("Time: %.0f\n", stoptimer_us());
	return 0;
}
