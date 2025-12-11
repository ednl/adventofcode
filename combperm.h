#ifndef COMBPERM_H
#define COMBPERM_H

// Successive calls give combinations of k indices from a set of n.
// Adapted from Knuth 4A, §7.2.1.3, algorithm T.
// Returns pointer to array of int, index 0..k-1.
// For example for n=3,k=2: combinations(3,2) gives [0,1], [0,2], [1,2]
// Call as combinations(0,0) to reset and free memory.
int *combinations(const int n, const int k);

// Successive calls give permutations in lexographic order of n index numbers.
// https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// NB: not thread-safe because permutations are stored in local static variable.
// Returns pointer to first element of next permutation of n index numbers.
// Call as permutations(0) to reset and free memory.
int *permutations(const int n);

#endif // COMBPERM_H
