from itertools import combinations
from math import prod

data = []
with open('../aocinput/2015-02-input.txt') as f:
    for line in f:
        data.append(sorted(map(int, line.strip().split('x'))))

# Part  1
def wrap(dim):
    return prod(dim[:2]) + 2 * sum(map(prod, combinations(dim, 2)))
print(sum(map(wrap, data)))

# Part 2
def ribbon(dim):
    return 2 * sum(dim[:2]) + prod(dim)
print(sum(map(ribbon, data)))
