import numpy as np
from scipy.signal import convolve2d

with open('../aocinput/2015-18-input.txt') as f:
    data = (np.array([list(i.strip()) for i in f]) == '#').astype(np.uint8)

# Leave centre bit on, so change neighbour check from 3,2 to 4,3
kernel = np.ones((3, 3), dtype=np.uint8)

# Part 1
state = np.copy(data)
for _ in range(100):
    nb = convolve2d(state, kernel, mode='same')
    state = state & (nb == 4) | (nb == 3)
print(np.sum(state))

# Part 2
state = np.copy(data)
state[0, 0] = state[0, 99] = state[99, 0] = state[99, 99] = 1
for _ in range(100):
    nb = convolve2d(state, kernel, mode='same')
    state = state & (nb == 4) | (nb == 3)
    state[0, 0] = state[0, 99] = state[99, 0] = state[99, 99] = 1
print(np.sum(state))
