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
    min_qe = float('+inf')
    while not found:
        for c in combinations(data, minlen):
            weight = sum(c)
            if weight == groupweight:
                found = True
                qe = prod(c)
                if qe < min_qe:
                    min_qe = qe
        if not found:
            minlen += 1
    return min_qe

print(quantum(3), quantum(4))
