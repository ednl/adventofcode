'''
Advent of Code 2025
Day 4: Printing Department
https://adventofcode.com/2025/day/4
By: E. Dronkert https://github.com/ednl
'''

from time import monotonic_ns
t0 = monotonic_ns()

import numpy as np
from scipy.signal import convolve2d

t1 = monotonic_ns()

# convert to array filled with 0 (=floor/space '.') or 1 (=paper roll '@')
# with open('../aocinput/2025-04-example.txt') as f:
with open('../aocinput/2025-04-input.txt') as f:
    state = (np.array([list(i.strip()) for i in f]) == '@').astype(np.uint8)

kernel = np.ones((3, 3), dtype=np.uint8)
kernel[1, 1] = 0

def forklift():
    global state
    nb = convolve2d(state, kernel, mode='same')  # neighbour count per cell
    state = state & (nb > 3)  # leave rolls that have more than 3 neighbours
    return np.sum(state)      # new number of paper rolls

t2 = monotonic_ns()

init = np.sum(state)  # initial number of paper rolls
count = forklift()    # number of paper rolls after 1 iteration
print(f"Part 1: {init - count}")  # number of paper rolls removed after 1 iteration
# example: 13, input: 1495

t3 = monotonic_ns()

prev = 0
while count != prev:
    prev = count
    count = forklift()
print(f"Part 2: {init - count}")  # total number of paper rolls removed
# example: 43, input: 8768

t4 = monotonic_ns()

print('import libs : {:5.1f} ms'.format((t1 - t0)/1000000))
print('parse input : {:5.1f} ms'.format((t2 - t1)/1000000))
print('part 1      : {:5.1f} ms'.format((t3 - t2)/1000000))
print('part 2      : {:5.1f} ms'.format((t4 - t3)/1000000))
print('parts 1+2   : {:5.1f} ms'.format((t4 - t2)/1000000))
print('parse+p1+p2 : {:5.1f} ms'.format((t4 - t1)/1000000))
print('all         : {:5.1f} ms'.format((t4 - t0)/1000000))
