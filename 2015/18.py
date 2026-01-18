from time import monotonic_ns
t0 = monotonic_ns()

import numpy as np
from scipy.signal import convolve2d

t1 = monotonic_ns()

# From puzzle description
STEP = 100
SIZE = 100
MAX = SIZE - 1  # zero-based array

with open('../aocinput/2015-18-input.txt') as f:
    data = (np.array([list(i.strip()) for i in f]) == '#').astype(np.uint8)

# Leave centre bit on, so change neighbour check from 3,2 to 4,3
kernel = np.ones((3, 3), dtype=np.uint8)

t2 = monotonic_ns()

# Part 1
state = np.copy(data)
for _ in range(STEP):
    nb = convolve2d(state, kernel, mode='same')
    state = state & (nb == 4) | (nb == 3)
print(np.sum(state))  # 821

t3 = monotonic_ns()

# Part 2
state = np.copy(data)
state[0, 0] = state[0, MAX] = state[MAX, 0] = state[MAX, MAX] = 1
for _ in range(STEP):
    nb = convolve2d(state, kernel, mode='same')
    state = state & (nb == 4) | (nb == 3)
    state[0, 0] = state[0, MAX] = state[MAX, 0] = state[MAX, MAX] = 1
print(np.sum(state))  # 886

t4 = monotonic_ns()
print()
print('Import     : {:4.0f} ms'.format((t1 - t0) / 1_000_000))
print('Read+parse : {:4.0f} ms'.format((t2 - t1) / 1_000_000))
print('Part 1     : {:4.0f} ms'.format((t3 - t2) / 1_000_000))
print('Part 2     : {:4.0f} ms'.format((t4 - t3) / 1_000_000))
print('All        : {:4.0f} ms'.format((t4 - t0) / 1_000_000))
print('No import  : {:4.0f} ms'.format((t4 - t1) / 1_000_000))
