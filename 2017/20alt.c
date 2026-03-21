#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdbool.h>

#define N 1000
#define M 16

typedef struct vec3 {
    int x, y, z;
} Vec3;

typedef struct particle {
    Vec3 p, v, a;
} Particle;

typedef struct state {
    int p, v, a;
} State;

static Particle particle[N];
static int lowestaccel[M];
static bool left[N];  // particle is left after collisions resolved

static void show(const Particle *const p)
{
    printf("p=<%d,%d,%d>, v=<%d,%d,%d>, a=<%d,%d,%d>\n",
        p->p.x, p->p.y, p->p.z,
        p->v.x, p->v.y, p->v.z,
        p->a.x, p->a.y, p->a.z);
}

static int manh(const Vec3 v)
{
    return abs(v.x) + abs(v.y) + abs(v.z);
}

static Vec3 add(const Vec3 v, const Vec3 w)
{
    return (Vec3){v.x + w.x, v.y + w.y, v.z + w.z};
}

static Vec3 sub(const Vec3 v, const Vec3 w)
{
    return (Vec3){v.x - w.x, v.y - w.y, v.z - w.z};
}

static Vec3 mul(const Vec3 v, const int c)
{
    return (Vec3){v.x * c, v.y * c, v.z * c};
}

static bool collide(const State s1, const State s2)
{
    //
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
    int min = 1000000;
    int count = 0;
    for (int i = 0; i < n; ++i) {
        const int a = manh(particle[i].a);  // a instead of a/2 to avoid fraction
        if (a < min) {
            min = a;
            lowestaccel[0] = i;
            count = 1;
        } else if (a == min && count < M)
            lowestaccel[count++] = i;
    }

    min = 1000000;
    int closest = -1;
    for (int i = 0; i < count; ++i) {
        const Particle *const p = &particle[lowestaccel[i]];
        const int decider = manh(add(mul(p->v, 2), p->a));  // 2v+a instead of v+a/2 to avoid fraction
        if (decider < min) {
            min = decider;
            closest = lowestaccel[i];
        }
    }
    printf("%d\n", closest);  // 161
}
