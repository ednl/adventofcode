#include <stdio.h>   // fgetc, fscanf
#include <stdlib.h>  // qsort

#define N 1200
#define M 400

typedef enum {
    None, Change, Sleep, Wake
} EventType;

// Observation
typedef struct {
    unsigned int year, mon, day, hour, min;
    EventType event;
    unsigned int id;
} Obsv;

typedef struct {
    size_t index;
    unsigned int id, sum;
    unsigned char bin[60];
} Hist;

static Obsv obsv[N] = {0};
static unsigned int guardid[M] = {0};
static Hist hist[M] = {0};

static int cmp_obsv(const void *a, const void *b)
{
    const Obsv *const p = a;
    const Obsv *const q = b;
    if (p->year < q->year) return -1;
    if (p->year > q->year) return  1;
    if (p->mon  < q->mon ) return -1;
    if (p->mon  > q->mon ) return  1;
    if (p->day  < q->day ) return -1;
    if (p->day  > q->day ) return  1;
    if (p->hour < q->hour) return -1;
    if (p->hour > q->hour) return  1;
    if (p->min  < q->min ) return -1;
    if (p->min  > q->min ) return  1;
    return 0;
}

static int cmp_int(const void *a, const void *b)
{
    const unsigned int p = *(const unsigned int *const)a;
    const unsigned int q = *(const unsigned int *const)b;
    if (p < q) return -1;
    if (p > q) return  1;
    return 0;
}

static int cmp_hist(const void *a, const void *b)
{
    const Hist *const p = a;
    const Hist *const q = b;
    if (p->id < q->id) return -1;
    if (p->id > q->id) return  1;
    return 0;
}

int main(void)
{
    unsigned int year, mon, day, hour, min;
    size_t i, j, n = 0, m = 0;

    FILE *f = fopen("../aocinput/2018-04-input.txt", "r");
    if (!f)
        return 1;

    // Read all events, save every guard ID
    while (n < N && fscanf(f, "[%u-%u-%u %u:%u] ", &year, &mon, &day, &hour, &min) == 5) {
        EventType event = None;
        unsigned int id = 0;
        int c = fgetc(f);
        switch (c) {
            case 'G':
                if (fscanf(f, "uard #%u ", &id) == 1) {
                    event = Change;
                    if (m < M)
                        guardid[m++] = id;
                } else {
                    event = None;
                    id = 0;
                }
                break;
            case 'f': event = Sleep; break;
            case 'w': event = Wake; break;
        }
        obsv[n++] = (Obsv){year, mon, day, hour, min, event, id};
        while (c != '\n' && c != EOF) {
            c = fgetc(f);
        }
    }
    fclose(f);

    // Unique Guard IDs
    qsort(guardid, m, sizeof(unsigned int), cmp_int);
    i = 0;
    j = 1;
    while (j < m) {
        while (j < m && guardid[j] == guardid[i]) {
            ++j;
        }
        if (j < m) {
            guardid[++i] = guardid[j++];
        }
    }
    m = i + 1;

    // Put unique guard IDs in the histogram in order
    for (i = 0; i < m; ++i) {
        hist[i].index = i;
        hist[i].id = guardid[i];
    }

    // Sort all events by time, add sleeping minutes to histogram, keep track of which guard slept most
    qsort(obsv, n, sizeof(Obsv), cmp_obsv);
    Hist key = {0}, *res = NULL;
    unsigned int maxsum = 0;
    size_t maxindex = 0;
    for (i = 0; i < n; ++i) {
        switch (obsv[i].event) {
            case Change:
                key.id = obsv[i].id;
                res = bsearch(&key, hist, m, sizeof(Hist), cmp_hist);
                break;
            case Wake:
                if (res) {
                    for (j = obsv[i - 1].min; j < obsv[i].min; ++j) {
                        res->bin[j]++;
                    }
                    res->sum += obsv[i].min - obsv[i - 1].min;
                    if (res->sum > maxsum) {
                        maxsum = res->sum;
                        maxindex = res->index;
                    }
                }
                break;
            case None:
            case Sleep:
                break;
        }
    }

    // Find the minute with most sleep for the guard who slept most
    size_t maxminute = 0;
    maxsum = 0;
    for (i = 0; i < 60; ++i) {
        if (hist[maxindex].bin[i] > maxsum) {
            maxsum = hist[maxindex].bin[i];
            maxminute = i;
        }
    }
    printf("Part 1: %zu\n", hist[maxindex].id * maxminute);

    maxindex = 0;
    maxminute = 0;
    maxsum = 0;
    for (i = 0; i < m; ++i) {
        for (j = 0; j < 60; ++j) {
            if (hist[i].bin[j] > maxsum) {
                maxindex = i;
                maxminute = j;
                maxsum = hist[i].bin[j];
            }
        }
    }
    printf("Part 2: %zu\n", hist[maxindex].id * maxminute);

    return 0;
}
