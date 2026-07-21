#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FNAME "../aocinput/2021-20-input.txt"
#define BITS 512
#define DIM  100
#define SIZE 256
#define FSIZE (DIM * (DIM + 1) + BITS + 2)  // 10614

typedef struct pos {
    int dr, dc;
} Pos;
static const Pos kernel[9] = {
    {-1,-1},{-1,0},{-1,1},
    { 0,-1},{ 0,0},{ 0,1},
    { 1,-1},{ 1,0},{ 1,1},
};

static char input[FSIZE];
static bool filter[BITS];
static bool img[2][SIZE][SIZE];  // current and next image
static int cur = 0, zero = (SIZE - DIM) >> 1, lim = (SIZE + DIM) >> 1;

static void evolve(const int nextgen)
{
    for (int gen = 1; gen <= nextgen; ++gen) {
        const int next = gen & 1;
        const int zero2 = zero - 1;
        const int lim2 = lim + 1;
        const bool inf = cur;
        for (int i = zero2; i < lim2; ++i) {  // rows in next image
            for (int j = zero2; j < lim2; ++j) {  // cols in next image
                int index = 0;
                for (int k = 0; k < 9; ++k) {
                    index <<= 1;
                    int r = i + kernel[k].dr;  // row in cur image
                    int c = j + kernel[k].dc;  // col in cur image
                    index |= r >= zero && r < lim && c >= zero && c < lim ? img[cur][r][c] : inf;
                }
                img[next][i][j] = filter[index];
            }
        }
        cur = next;
        zero = zero2;
        lim = lim2;
    }
}

static int lit(void)
{
    int sum = 0;
    for (int i = zero; i < lim; ++i)
        for (int j = zero; j < lim; ++j)
            sum += img[cur][i][j];
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) return 1;
    fread(input, FSIZE, 1, f);  // read whole file as one single block of size=FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse
    const char *c = input;
    for (int i = 0; i < BITS; ++i)
        filter[i] = *c++ == '#';
    c += 2;
    for (int i = zero; i < lim; ++c, ++i)
        for (int j = zero; j < lim; ++j)
            img[0][i][j] = *c++ == '#';

    evolve(2);
    printf("Part 1: %d\n", lit());  // 5229
    evolve(48);
    printf("Part 2: %d\n", lit());  // 17009

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
