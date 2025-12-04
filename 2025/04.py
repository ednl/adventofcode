import numpy as np
from scipy.signal import convolve2d

with open('../aocinput/2025-04-input.txt') as f:
    state = (np.array([list(i.strip()) for i in f]) == '@').astype(np.uint8)

# Leave centre bit on, so change neighbour check from <4 to <5
kernel = np.ones((3, 3), dtype=np.uint8)

prev = 0
init = count = np.sum(state)
while prev != count:
    prev = count
    nb = convolve2d(state, kernel, mode='same')
    state = state & (nb > 4)  # leave rolls that have more than 3 neighbours
    count = np.sum(state)
print(init - count)  # total removed
