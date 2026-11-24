#include <stdio.h>
#include <stdint.h>

#define FNAME "../aocinput/2025-08-input.txt"
#define N 1000
#define FSIZE (N * 3 * (5 + 1))

typedef int64_t i64;

typedef struct vec {
    i64 x, y, z;
} Vec;

static char input[FSIZE];
static Vec box[N];

static int readnum(const char **const s)
{
    int x = 0;
    while (**s & 16)  // until comma or newline
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip comma or newline
    return x;
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

static i64 sqrsum(const Vec a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

static i64 sqrdist(const Vec a, const Vec b)
{
    return sqrsum(sub(a, b));
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME"\n", stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

    const char *s = input;
    for (int i = 0; i < N; ++i) {
        box[i].x = readnum(&s);
        box[i].y = readnum(&s);
        box[i].z = readnum(&s);
    }

    i64 part2 = 0, last = 0;
    for (int i = 0; i < N; ++i) {
        int k = 0;
        i64 min = INT64_MAX;
        for (int j = 0; j < N; ++j)
            if (i != j) {
                i64 d = sqrdist(box[i], box[j]);
                if (d < min) {
                    min = d;
                    k = j;
                }
            }
        if (min > last) {
            last = min;
            part2 = box[i].x * box[k].x;
        }
    }
    printf("%llu\n", part2);  // 17001512, should be: 772452514
}
