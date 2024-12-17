#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64

typedef uint64_t u64;
typedef struct prog {
    u64 val;
    int len;
} Prog;

static const char *prog_ex1 = "0,1,5,4,3,0";
static const char *prog_ex2 = "0,3,5,4,3,0";
static const char *prog_inp = "2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0";

// Number of 3-bit chunks
// static inline int chunks(u64 x)
// {
//     return x ? (66 - __builtin_clzll(x)) / 3 : 1;
// }

// static void bin(const u64 x)
// {
//     u64 bit = 1ULL << 62;
//     while (bit) {
//         putchar(' ');
//         putchar('0' + (int)(x & bit)); bit >>= 1;
//         putchar('0' + (int)(x & bit)); bit >>= 1;
//         putchar('0' + (int)(x & bit)); bit >>= 1;
//     }
//     putchar('\n');
// }

// Return CSV string of 3-bit chunks of p.val, from MSB to LSB
static void out(Prog p)
{
    char csv[64];
    int i = p.len * 2;
    csv[--i] = '\0';
    csv[--i] = "01234567"[p.val & 7];
    while (i) {
        csv[--i] = ',';
        csv[--i] = "01234567"[(p.val >>= 3) & 7];
    }
    puts(csv);
}

static Prog inp(const char *csv)
{
    Prog p = {0};
    if (csv && *csv) {
        p.val = *csv++ & 15;
        p.len = 1;
        while (*csv++ == ',') {
            p.val = p.val << 3 | (*csv++ & 15);
            p.len++;
        }
    }
    return p;
}

// prog: 0,1,5,4,3,0
static Prog ex1(u64 a)
{
    Prog p = {0};
    do {
        a >>= 1;                       // 0: adv 1
        p.val = p.val << 3 | (a & 7);  // 2: out A
        p.len++;
    } while (a);                       // 4: jnz 0
    return p;
}

// prog: 0,3,5,4,3,0
static Prog ex2(u64 a)
{
    Prog p = {0};
    do {
        a >>= 3;                       // 0: adv 3
        p.val = p.val << 3 | (a & 7);  // 2: out A
        p.len++;
    } while (a);                       // 4: jnz 0
    return p;
}

// prog: 2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0
static Prog fwd(u64 a)
{
    Prog p = {0};
    u64 b = 0, c = 0;
    do {
        b = a & 7;                     //  0: bst A
        b ^= 2;                        //  2: bxl 2 (B=0..7 -> 2,3,0,1,6,7,4,5)
        c = a >> b;                    //  4: cdv B
        b ^= c;                        //  6: bxc
        b ^= 3;                        //  8: bxl 3 (B=0..7 -> 3,2,1,0,7,6,5,4)
        p.val = p.val << 3 | (b & 7);  // 10: out B
        p.len++;
        a >>= 3;                       // 12: adv 3
    } while (a);                       // 14: jnz 0
    return p;
}

static u64 rev(Prog p)
{
    u64 a = 0, b = 0, c = 0;
    while (p.len--) {
        b = p.val & 7;
        p.val >>= 3;
        b ^= 3;
        //TODO
    }
    return a;
}

int main(void)
{
    // Test
    out(inp(prog_ex1));
    out(inp(prog_ex2));
    out(inp(prog_inp));

    // Example 1
    out(ex1(729));     // 4,6,3,5,6,3,5,2,1,0

    // Part 1
    Prog p = fwd(35200350);
    out(p);            // 2,7,4,7,2,1,7,5,1

    // Example 2
    out(ex2(117440));  // 0,3,5,4,3,0

    // Part 2
    printf("%"PRIu64"\n", rev(p));  // ?
    return 0;
}
