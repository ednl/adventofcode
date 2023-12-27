import re
coor = re.compile(r' (\d+),(\d+) through (\d+),(\d+)\n')

import numpy as np
lights1 = np.zeros((1000,1000), dtype=np.int8)
lights2 = np.zeros((1000,1000), dtype=np.int8)

with open('../aocinput/2015-06-input.txt') as f:
    for line in f:
        m = coor.search(line)  # match coordinates
        if m:
            p = list(map(int, m.group(1,2,3,4)))
            p[2] += 1  # inclusive to exclusive range
            p[3] += 1
            if line[6] == 'n':  # turn on
                lights1[p[0]:p[2], p[1]:p[3]] = 1
                lights2[p[0]:p[2], p[1]:p[3]] += 1
            elif line[6] == 'f':  # turn off
                lights1[p[0]:p[2], p[1]:p[3]] = 0
                lights2[p[0]:p[2], p[1]:p[3]] -= 1
                lights2[lights2 < 0] = 0
            elif line[6] == ' ':  # toggle
                lights1[p[0]:p[2], p[1]:p[3]] ^= 1
                lights2[p[0]:p[2], p[1]:p[3]] += 2
print(np.sum(lights1))
print(np.sum(lights2))
