#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// #define DEBUG

#define L    26
#ifdef DEBUG
#define W    2
#define COST 1
#else
#define W    5
#define COST 61
#endif

typedef struct {
    int count;
    bool used;
    bool req[L];
} Dep;

typedef struct {
    int point, timer;
    bool busy;
} Worker;

static Dep dep[L] = {0};
static Worker worker[W] = {0};

static int readrules(void)
{
#ifdef DEBUG
    FILE *f = fopen("../aocinput/2018-07-example.txt", "r");
#else
    FILE *f = fopen("../aocinput/2018-07-input.txt", "r");
#endif
    if (!f)
        return -1;

    int i, j;
    for (i = 0; i < L; ++i) {
        dep[i].count = 0;
        dep[i].used = false;
        for (j = 0; j < L; ++j) {
            dep[i].req[j] = false;
        }
    }

    char a, b;
    int n = 0;
    while (n <= L && fscanf(f, "Step %c must be finished before step %c can begin. ", &a, &b) == 2) {
        i = a - 'A';
        j = b - 'A';
        if (i >= 0 && i < L && j >= 0 && j < L && !dep[j].req[i]) {
            dep[j].count++;
            dep[j].req[i] = true;
            if (i >= n) {
                n = i + 1;
            }
            if (j >= n) {
                n = j + 1;
            }
        }
    }
    fclose(f);
    return n;
}

int main(void)
{
    printf("Part 1: ");
    int len = readrules(), i = 0, j;
    while (i < len) {
        // Find first point with no requirements and not used yet
        while (i < len && (dep[i].count || dep[i].used))
            ++i;
        if (i < len) {
            // If found, then use that point and update requirements of other points
            printf("%c", i + 'A');
            dep[i].used = true;
            for (j = 0; j < len; ++j) {
                if (dep[j].req[i]) {
                    dep[j].req[i] = false;
                    dep[j].count--;
                }
            }
            // Redo from start because it must be alphabetically
            i = 0;
        }
    }
    printf("\n");  // FHICMRTXYDBOAJNPWQGVZUEKLS

    int todo = readrules();
    int w, t = 0;
    while (todo > 0) {
        // Offload workers
        for (w = 0; w < W; ++w) {
            if (worker[w].busy) {
                if (--worker[w].timer == 0) {
                    worker[w].busy = false;
                    --todo;
                    // Adjust requirements
                    for (j = 0; j < len; ++j) {
                        if (dep[j].req[worker[w].point]) {
                            dep[j].req[worker[w].point] = false;
                            dep[j].count--;
                        }
                    }
                }
            }
        }
        if (todo > 0) {
            // Distribute new load to workers
            w = i = 0;
            while (w < W && i < len) {
                // Find next free worker
                while (w < W && worker[w].busy)
                    ++w;
                if (w < W) {
                    // Find next free point
                    while (i < len && (dep[i].count || dep[i].used))
                        ++i;
                    if (i < len) {
                        worker[w] = (Worker){i, i + COST, true};
                        dep[i].used = true;
                        ++w;
                        ++i;
                    }
                }
            }
            ++t;  // advance time
        }
    }
    printf("Part 2: %d\n", t);  // 946
    return 0;
}
