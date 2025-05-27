from itertools import combinations
from math import prod

with open('../aocinput/2015-24-input.txt') as f:
    data = sorted(map(int, list(f)), reverse=True)

def quantum(groups: int) -> int:
    groupweight = sum(data) // groups
    minlen = 1
    while sum(data[:minlen]) < groupweight:
        minlen += 1
    found = False
    min_qe = 0
    while not found:
        for c in combinations(data, minlen):
            weight = sum(c)
            if weight == groupweight:
                qe = prod(c)
                if qe < min_qe or not found:
                    min_qe = qe
                    found = True
        minlen += 1
    return min_qe

print(quantum(3), quantum(4))
