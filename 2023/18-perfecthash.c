#include <stdio.h>

#define R 82U
#define D 68U
#define L 76U
#define U 85U

static unsigned test(const unsigned mul, const unsigned ofs)
{
    unsigned r = R * mul + ofs;
    unsigned d = D * mul + ofs;
    unsigned l = L * mul + ofs;
    unsigned u = U * mul + ofs;
    for (unsigned shr = 0; l && u; shr++) {
        if ((r & 3) == 0 && (d & 3) == 1 && (l & 3) == 2 && (u & 3) == 3) {
            printf("%3u %4u %u\n", mul, ofs, shr);
            return 1;
        }
        r >>= 1;
        d >>= 1;
        l >>= 1;
        u >>= 1;
    }
    return 0;
}

int main(void)
{
    for (unsigned a = 1; a < 1000; ++a)
        for (unsigned b = 0; b < 100000; ++b)
            if (test(a, b))
                break;
}
