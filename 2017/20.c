#include <stdio.h>   // printf, scanf
#include <stdlib.h>  // abs, exit
#include <math.h>    // round, fabs, sqrt

#define DIM        (3)     // x,y,z
#define PARTICLESZ (1000)  // exact number of lines in my input
#define COLPAIRSZ  (2000)  // actually about 1670 for my input

typedef struct particle {
    int acc[DIM], vel[DIM], pos[DIM]; // motion quantities
    int col;  // collision time (initially: -1 = never)
    int ord;  // ordinal number (= zero-based order in input file)
} Particle;
static Particle particles[PARTICLESZ] = {0};

// Collision pairs
typedef struct colpair {
    int t;       // collision time
    int n1, n2;  // particle numbers
} ColPair;
static ColPair colpairs[COLPAIRSZ] = {0};

// Read puzzle input
static int load(void)
{
    FILE *f = fopen("../aocinput/2017-20-input.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    int n = 0;
    Particle p = {0};
    while (fscanf(f, "p=<%d,%d,%d>, v=<%d,%d,%d>, a=<%d,%d,%d> ",
        &p.pos[0], &p.pos[1], &p.pos[2],
        &p.vel[0], &p.vel[1], &p.vel[2],
        &p.acc[0], &p.acc[1], &p.acc[2]) == 9) {
        if (n == PARTICLESZ) {
            fprintf(stderr, "particles[] too small\n");
            fclose(f);
            exit(2);
        }
        p.col = -1;  // never
        p.ord = n;
        particles[n++] = p;
    }
    fclose(f);
    return n;
}

// Add vector to vector
static inline void addv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] + b[i];
    }
}

// Multiply vector by scalar
static inline void muls(int *r, const int *a, const int n)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] * n;
    }
}

// Manhattan distance
static inline int manh(const int *a)
{
    return abs(a[0]) + abs(a[1]) + abs(a[2]);
}

// Sort particles by acceleration, then speed + half acc., then position
// (all in terms of Manhattan distance because that is the criterium)
//   p(t) = p + v.t + a.t.(t+1)/2
//        = p + (v + a/2).t + a.t.t/2
static int cmp1(const void *a, const void *b)
{
    const Particle *p = (const Particle *)a, *q = (const Particle *)b;
    int n = manh(p->acc) - manh(q->acc);
    if (n == 0) {
        int t[DIM];
        muls(t, p->vel, 2);
        addv(t, p->acc, t);
        n = manh(t);
        muls(t, q->vel, 2);
        addv(t, q->acc, t);
        n -= manh(t);
        if (n == 0) {
            return manh(p->pos) - manh(q->pos);
        }
    }
    return n;
}

// Sort particles by ordinal (= reset previous sort)
static int cmp2(const void *a, const void *b)
{
    return ((const Particle *)a)->ord - ((const Particle *)b)->ord;
}

// Return non-negative integer equal to a, or -1
static inline int nonnegint(double a)
{
    double b = round(a);
    int i = fabs(a - b) < 0.000001 ? (int)b : -1;
    return i >= 0 ? i : -1;
}

// Does new time solution t fit with previous solutions a and b?
// If not, return -1
static inline int check1(int a, int b, int t)
{
    return t >= 0 && (t == a || t == b || a == -2) ? t : -1;
}

// Do new time solutions t1,t2 fit with previous solutions a and b?
// If not, set a and b to -1
static void check2(int *a, int *b, int t1, int t2)
{
    if (t1 < 0 && t2 < 0) {              // no collision possible
        *a = *b = -1;
    } else if (t1 < 0) {                 // collision possible at t2
        *a = *b = check1(*a, *b, t2);
    } else if (t2 < 0 || t1 == t2) {     // collision possible at t1
        *a = *b = check1(*a, *b, t1);
    } else if (*a == -2) {               // no previous restrictions
        *a = t1;
        *b = t2;
    } else if (*a == t1 && *b != t2) {  // t1 was a previous solution (a) but t2 was not
        *b = t1;
    } else if (*a == t2 && *b != t1) {  // t2 was a previous solution (a) but t1 was not
        *b = t2;
    } else if (*b == t1 && *a != t2) {  // t1 was a previous solution (b) but t2 was not
        *a = t1;
    } else if (*b == t2 && *a != t1) {  // t2 was a previous solution (b) but t1 was not
        *a = t2;
    } else if (!((*a == t1 && *b == t2) || (*a == t2 && *b == t1))) {  // neither t1 nor t2 were previous solutions
        *a = *b = -1;
    }
}

// Returns earliest time of collision (no collision = -1)
//       p1(t) = p2(t)
//   <=> p1 + v1.t + a1.t.(t+1)/2 = p2 + v2.t + a2.t.(t+1)/2
//   <=> p1 + (v1 + a1/2).t + a1.t.t/2 = p2 + (v2 + a2/2).t + a2.t.t/2
//   <=> (a1 - a2)/2.t.t + (v1 - v2 + (a1 - a2)/2).t + (p1 - p2) = 0
// Case 1: (a1 - a2) == 0 => (v1 - v2).t + (p1 - p2) = 0
//     Case 1a: (v1 - v2) == 0 => all t if (p1 - p2) == 0 else no solution
//     Case 1b: (v1 - v2) != 0 => t = -(p1 - p2)/(v1 - v2) if integer else no solution
// Case 2: (a1 - a2) != 0 => t.t + (2.(v1 - v2)/(a1 - a2) + 1).t + 2.(p1 - p2)/(a1 - a2) = 0
//     (Coefficients may be fractions, integer solutions still possible!)
//     "a" = 1
//     "b" = 2.(v1 - v2)/(a1 - a2) + 1
//     "m" = -b/2 = (v2 - v1)/(a1 - a2) - 0.5
//     "c" = 2.(p1 - p2)/(a1 - a2)
//     "D" = b.b - 4.a.c = 4.p.p - 4.c = (p.p - c).4
//     "E" = D/4 = m.m - c
//     Case 2a: E == 0 => t = -b/(2.a) = m
//     Case 2b: E > 0  => t0,1 = (-b +- sqrt(D))/(2.a) = m +- sqrt(E)
//     Case 2c: E < 0  => no solution
static int collide(const Particle *p1, const Particle *p2)
{
    int t0 = -2, t1 = -2;  // -2=always overlap, -1=never collide

    for (int i = 0; i < DIM; ++i) {
        int da = p1->acc[i] - p2->acc[i];
        int dv = p2->vel[i] - p1->vel[i];  // -(v1 - v2) = (v2 - v1)
        int dp = p1->pos[i] - p2->pos[i];
        if (da == 0) {
            if (dv == 0) {
                if (dp != 0) {
                    return -1;
                }
            } else {
                if (dp % dv) {
                    return -1;
                }
                if ((t0 = t1 = check1(t0, t1, dp / dv)) == -1) {
                    return -1;
                }
            }
        } else {
            double m = (double)dv / da - 0.5;  // m = -b/2
            double c = (double)(dp * 2) / da;
            double E = m * m - c;              // E = D/4
            if (fabs(E) < 0.000001) {          // discriminant == 0
                if ((t0 = t1 = check1(t0, t1, nonnegint(m))) == -1) {
                    return -1;
                }
            } else if (E > 0) {
                double root = sqrt(E);         // sqrt(E) = sqrt(D)/2
                check2(&t0, &t1, nonnegint(m - root), nonnegint(m + root));
                if (t0 == -1) {
                    return -1;
                }
            } else {
                return -1;
            }
        }
    }
    return t0 == -2 ? 0 : t0;  // but t0 is never -2 for my puzzle input
}

// Sort collision pairs by time, ord1, ord2
static int cmpcol(const void *a, const void *b)
{
    const ColPair *p = (const ColPair *)a, *q = (const ColPair *)b;
    int d = p->t - q->t;
    if (d == 0) {
        d = p->n1 - q->n1;
        if (d == 0) {
            return p->n2 - q->n2;
        }
    }
    return d < 0 ? -1 : 1;
}

int main(void)
{
    int particlecount = load();

    // Part 1
    qsort(particles, (size_t)particlecount, sizeof(Particle), cmp1);
    printf("Part 1: %d\n", particles[0].ord);

    // Reset order => particles[i].n == i
    qsort(particles, (size_t)particlecount, sizeof(Particle), cmp2);

    // Part 2
    int colcount = 0;
    for (int i = 0; i < particlecount - 1; ++i) {
        for (int j = i + 1; j < particlecount; ++j) {
            int t = collide(&particles[i], &particles[j]);
            if (t >= 0) {
                if (colcount == COLPAIRSZ) {
                    fprintf(stderr, "colpairs[] too small\n");
                    exit(3);
                }
                colpairs[colcount++] = (ColPair){
                    .t = t,
                    .n1 = particles[i].ord,
                    .n2 = particles[j].ord,
                };
            }
        }
    }

    // Sort collisions by ascending time, ord1, ord2
    qsort(colpairs, (size_t)colcount, sizeof(ColPair), cmpcol);

    // Strike out particle pairs that collided, but only if they haven't collided yet
    for (int i = 0; i < colcount; ++i) {
        int t = colpairs[i].t;
        Particle *p1 = &particles[colpairs[i].n1];
        Particle *p2 = &particles[colpairs[i].n2];
        // Never collided before or just collided on this same time
        if ((p1->col < 0 || p1->col == t) && (p2->col < 0 || p2->col == t)) {
            p1->col = p2->col = t;
        }
    }

    // Count remaining particles
    int alive = 0;
    for (int i = 0; i < particlecount; ++i) {
        alive += (particles[i].col < 0);
    }
    printf("Part 2: %d\n", alive);  // right answer = 438 for my input
    return 0;
}
