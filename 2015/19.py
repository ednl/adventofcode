rules = {}
med = ''
with open('../aocinput/2015-19-input.txt') as f:
    data = f.read().split('\n\n')
    for line in data[0].splitlines():
        k, v = line.split(' => ', maxsplit=1)
        if k in rules:
            rules[k].append(v)
        else:
            rules[k] = [v]
    med = data[1].strip()

# Part 1
mut = set()
for k, v in rules.items():
    i = med.find(k)
    while i != -1:
        j = i + len(k)
        for r in v:
            mut.add(''.join((med[:i], r, med[j:])))
        i = med.find(k, j)
print("Part 1:", len(mut))  # Part 1: 518

# Part 2
# Hints: https://www.reddit.com/r/adventofcode/comments/3xflz8/day_19_solutions/

import re
elms = len(re.findall(r'[A-Z][a-z]?', med))
parns = len(re.findall(r'Rn|Ar', med))
commas = len(re.findall(r'Y', med))
print("Part 2: %d - %d - 2*%d - 1 = %d" % (elms, parns, commas, elms - parns - 2 * commas - 1))  # Part 2: 200
