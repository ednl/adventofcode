#include <stdlib.h>  // malloc, free
#include "combperm.h"

int *combinations(const int n, const int k)
{
    static int *index = NULL;
    static int j, x;

    // Wrong input or reset
    if (n <= 0 || k <= 0 || n <= k) {
        free(index);
        return index = NULL;
    }

    // Initialisation
    if (!index) {
        index = malloc((k + 2) * sizeof *index);
        if (index == NULL)
            return NULL;
        for (int i = 0; i < k; ++i)
            index[i] = i;
        index[k    ] = n;
        index[k + 1] = 0;
        j = k - 1;
        return index;
    }

    while (j >= 0) {
        index[j] = j + 1;
        j--;
        return index;
    }

    if (index[0] + 1 < index[1]) {
        index[0]++;
        return index;
    }

    j = 0;
    do {
        j++;
        index[j - 1] = j - 1;
    } while ((x = index[j] + 1) == index[j + 1]);

    if (j >= k) {
        free(index);
        return index = NULL;
    }

    index[j--] = x;
    return index;
}

// Local helper function: swap two ints via different pointers.
static void swap(int *const restrict a, int *const restrict b)
{
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

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
