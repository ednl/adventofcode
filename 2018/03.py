import re
import numpy as np

fmt = re.compile(r'^#(\d+) @ (\d+),(\d+): (\d+)x(\d+)')
with open('../aocinput/2018-03-input.txt') as f:
    claim = np.array(list(list(map(int, fmt.match(line).groups())) for line in f)).astype(np.uint16)

# Lay claims
fabric = np.zeros((1000,1000), dtype=np.uint16)
for c in claim:
    fabric[c[1]:c[1] + c[3], c[2]:c[2] + c[4]] += 1

# Part 1
print((fabric > 1).sum())

# Part 2
for c in claim:
    if fabric[c[1]:c[1] + c[3], c[2]:c[2] + c[4]].sum() == c[3] * c[4]:
        print(c[0])
