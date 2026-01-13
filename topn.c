#include <stdlib.h>  // qsort, size_t
#include <string.h>  // memmove, memcpy
#include "topn.h"

void topn(void *base, const size_t n, const size_t nel, const size_t width,
          int (*cmp)(const void *, const void *))
{
    // Sanity check, avoid undefined behaviour
    if (!n || n > nel || !width || !base || !cmp)
        return;

    // Sort first 'n' elements using library function
    qsort(base, n, width, cmp);

    // Insertion sort with binary search for all other elements
    unsigned char *const cbase = base;
    const unsigned char *const beg = cbase + width * n;
    const unsigned char *const end = cbase + width * nel;
    unsigned char *const last = cbase + width * (n - 1);
    for (const unsigned char *cur = beg; cur != end; cur += width) {

        if (cmp(last, cur) <= 0)  // last comes before cur, or they are equal?
            continue;  // should be most common case if n much smaller than nel

        if (cmp(cur, cbase) < 0) {  // cur comes before base?
            memmove(cbase + width, cbase, last - cbase);
            memcpy(cbase, cur, width);
            continue;  // should be rare but has to be checked before binsearch
        }

        // Insertion point is now always after 'l', at or before 'r'
        size_t l = 0, r = n - 1;
        while (r - l > 1) {
            const size_t m = ((l + 1) >> 1) + (r >> 1);  // avoid overflow, bias towards r
            if (cmp(cur, cbase + width * m) < 0) r = m;
            else                                 l = m;
        }
        unsigned char *const ins = cbase + width * r;
        memmove(ins + width, ins, last - ins);
        memcpy(ins, cur, width);
    }
}
