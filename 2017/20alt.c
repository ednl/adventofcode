#include <stdio.h>
#include <stdlib.h>  // abs
#include <limits.h>  // INT_MAX
#include <math.h>    // floor, sqrt

#define N 1000
#define M 16

typedef enum solcount {
    ALL = -1, NONE = 0
} Solcount;

typedef struct solution {
    Solcount count;
    int root[2];
} Solution;

typedef union vec3 {
    int i[3];
    struct { int x, y, z; };
} Vec3;

typedef struct particle {
    Vec3 p, v, a;
} Particle;

static Particle particle[N];
static int slow[M];  // particle IDs with lowest manhattan acceleration
static int coltime[N][N];  // earliest time > 0 when particle i and j collide (0=no collision)

// Manhattan distance (1-norm)
static int manh(const Vec3 v)
{
    return abs(v.x) + abs(v.y) + abs(v.z);
}

// Vector addition
static Vec3 add(const Vec3 v, const Vec3 w)
{
    return (Vec3){{v.x + w.x, v.y + w.y, v.z + w.z}};
}

// Vector subtraction
static Vec3 sub(const Vec3 v, const Vec3 w)
{
    return (Vec3){{v.x - w.x, v.y - w.y, v.z - w.z}};
}

// Vector-scalar multiplication
static Vec3 mul(const Vec3 v, const int c)
{
    return (Vec3){{v.x * c, v.y * c, v.z * c}};
}

// Vector pairwise multiplication
static Vec3 pwmul(const Vec3 v, const Vec3 w)
{
    return (Vec3){{v.x * w.x, v.y * w.y, v.z * w.z}};
}

// Smallest integer solution > 0 using quadratic formula
// 0 = no positive real integer solution
static int abc(int a, int b, int c)
{
    if (a && b && c) {
        // at^2 + bt + c = 0
        // => t1,2 = (-b +/- sqrt(b^2 - 4ac))/2a
        //         = (b +/- sqrt(b^2 - 2ac))/2a for a=2a and b=-b
        a *= 2;
        b = -b;
        const int D = b * b - 2 * a * c;  // a=2a
        if (D < 0)
            return 0;  // no real solutions
        if (D == 0) {
            if ((a < 0) != (b < 0))  // b=-b
                return 0;  // no positive solution
            const div_t t = div(b, a);  // b=-b, a=2a
            if (t.rem)
                return 0;  // no integer solution
            return t.quot;
        }
        const int r = round(sqrt(D));
        if (r * r != D)
            return 0;  // no integer solution
        const int num1 = b - r;  // b=-b
        const int num2 = b + r;  // b=-b
        const int t1 = (num1 < 0) != (a < 0) || num1 % a ? 0 : num1 / a;  // a=2a
        const int t2 = (num2 < 0) != (a < 0) || num2 % a ? 0 : num2 / a;  // a=2a
        if (t1 && t2)
            return t1 < t2 ? t1 : t2;
        return t1 ? t1 : t2;
    }
    if (!a && b && c) {
        // bt + c = 0
        // t = -c/b
        //   = c/b for c=-c
        c = -c;
        return (b < 0) != (c < 0) || c % b ? 0 : c / b;
    }
    if (a && !b && c) {
        // at^2 + c = 0
        // => t^2 = -c/a
        // => t = sqrt(c/a) for c=-c
        c = -c;
        if ((a < 0) != (c < 0))
            return 0;
        const div_t t = div(c, a);  // c=-c
        if (t.rem)
            return 0;
        const int r = round(sqrt(t.quot));
        return r * r == t.quot ? r : 0;
    }
    if (a && b && !c)
        // at^2 + bt = 0
        // => at + b = 0 (disregard t=0)
        // => t = -b/a
        //      = b/a for b=-b
        b = -b;
        return (a < 0) != (b < 0) || b % a ? 0 : b / a;
    return 0;
}

int main(void)
{
    FILE *fp = fopen("../aocinput/2017-20-input.txt", "r");
    int n = 0;
    for (int a,b,c,d,e,f,g,h,i; n < N &&
        fscanf(fp, "p=<%d,%d,%d>, v=<%d,%d,%d>, a=<%d,%d,%d> ",
        &a, &b, &c, &d, &e, &f, &g, &h, &i) == 9; ++n)
        particle[n] = (Particle){{a,b,c},{d,e,f},{g,h,i}};
    fclose(fp);

    // Part 1
    // p(t) = p + v.t + a.t.(t+1)/2
    //      = p + (v + a/2).t + a/2.t^2
    // Quadratic term in t leads, so find smallest a/2, then decide by v+a/2
    int min = INT_MAX;
    int count = 0;
    for (int i = 0; i < n; ++i) {
        const int a = manh(particle[i].a);  // a instead of a/2 to avoid fraction
        if (a < min) {
            min = a;
            slow[0] = i;
            count = 1;
        } else if (a == min && count < M)
            slow[count++] = i;
    }

    min = INT_MAX;
    int closest = -1;
    for (int i = 0; i < count; ++i) {
        const int j = slow[i];
        const int decider = manh(add(mul(particle[j].v, 2), particle[j].a));  // 2v+a instead of v+a/2 to avoid fraction
        if (decider < min) {
            min = decider;
            closest = j;
        }
    }
    printf("%d\n", closest);  // 161
}
