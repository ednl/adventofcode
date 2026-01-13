#ifndef TOPN_H
#define TOPN_H

#include <stddef.h>  // size_t

// Sort the first 'n' elements of array 'base' which has 'nel' elements of size
// 'width'. Sorting is done using the qsort-compatible comparison function 'cmp'
// Going in: 0 < sel <= nel, width > 0, base != NULL, cmp != NULL
// Going out: elements outside the top 'n' are not preserved
void topn(void *base, const size_t n, const size_t nel, const size_t width,
          int (*cmp)(const void *, const void *));

#endif //  TOPN_H
