#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

// Input file is 50,000 chars long (+newline)
#define N (50 * 1000)

typedef struct Polymer {
    struct Polymer *prev, *next;
    int unit;
    bool polarity;
} Polymer;

static Polymer polymer[N] = {0}, *head = NULL;

static int reduce(int remove)
{
    // Reset
    for (int i = 1; i < N; ++i) {
        polymer[i - 1].next = &polymer[i];
        polymer[i].prev = &polymer[i - 1];
    }
    head = polymer;
    head->prev = polymer[N - 1].next = NULL;

    // Remove one unit everywhere
    Polymer *p, *q;
    int n = N;
    if (remove) {
        while (head && head->unit == remove) {
            head = head->next;
            --n;
        }
        if (head) {
            head->prev = NULL;
            p = head->next;
            while (p) {
                while (p && p->unit == remove) {
                    q = p->prev;
                    q->next = (p = p->next);
                    if (p)
                        p->prev = q;
                    --n;
                }
                if (p)
                    p = p->next;
            }
        }
    }

    int oldn;
    do {
        oldn = n;
        p = head;
        q = p ? p->next : NULL;
        while (p && q) {
            while (p && q && p->unit == q->unit && p->polarity != q->polarity) {
                if (p->prev && q->next) {  // in the middle
                    p = p->prev;
                    q = q->next;
                    p->next = q;
                    q->prev = p;
                } else if (q->next) {  // at head end
                    q = (p = q->next)->next;
                    p->prev = NULL;
                    head = p;
                } else if (p->prev) {  // at tail end
                    p = (q = p->prev)->prev;
                    q->next = NULL;
                } else {
                    p = q = head = NULL;
                }
                n -= 2;
            }
            q = (p = q) ? p->next : NULL;
        }
    } while (oldn > n);
    return n;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2018-05-input.txt", "r");
    if (!f)
        return 1;

    int c, n = 0;
    while (n < N && (c = fgetc(f)) != EOF && c != '\n') {
        polymer[n].polarity = c == (polymer[n].unit = toupper(c));
        ++n;
    }
    fclose(f);
    if (n < N)
        return 2;

    printf("Part 1: %d\n", reduce(0));  // 11118

    int min = N;
    for (c = 'A'; c <= 'Z'; ++c) {
        n = reduce(c);
        if (n < min)
            min = n;
    }
    printf("Part 2: %d\n", min);  // 6948

    return 0;
}
