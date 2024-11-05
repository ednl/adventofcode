#include <stdlib.h>  // malloc, free
#include "permutations.h"

// Local helper function: swap two ints via different pointers.
static inline void swap(int *const restrict a, int *const restrict b)
{
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Successive calls give permutations in lexographic order of n index numbers.
// https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// NB: not thread-safe because permutations are stored in local static variable.
// Returns pointer to first element of next permutation of n index numbers.
int *permutations(const int n)
{
    static int *index = NULL;

    // Wrong input or manual reset: reset & return
    if (n < 1) {
        free(index);
        return index = NULL;
    }

    // Initialisation
    if (!index) {
        index = malloc(n * sizeof *index);
        if (!index)
            return NULL;
        for (int i = 0; i < n; ++i)
            index[i] = i;
        return index;
    }

    // Find last consecutive elements in increasing order
    int k = n - 2;
    while (k >= 0 && index[k] >= index[k + 1])
        --k;

    // If not found, this was the final permutation: reset & return
    if (k < 0) {
        free(index);
        return index = NULL;
    }

    // Find last element larger than index[k] and swap them
    int l = n - 1;
    while (l > k && index[k] >= index[l])
        --l;
    swap(&index[k], &index[l]);

    // Reverse sequence after index[k] and return permutation
    int i = k + 1, j = n - 1;
    while (i < j)
        swap(&index[i++], &index[j--]);
    return index;
}
