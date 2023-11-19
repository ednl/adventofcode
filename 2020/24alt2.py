import re
import numpy as np
from scipy.signal import convolve2d

# Part 1
step = {
    # Axial coordinates
    'e' : ( 1,  0),
    'w' : (-1,  0),
    'se': ( 0,  1),
    'nw': ( 0, -1),
    'ne': ( 1, -1),
    'sw': (-1,  1)
}
pat = re.compile('|'.join(step.keys()))    # works because findall() matches from start, non-overlapping
with open('../aocinput/2020-24-input.txt') as f:             # sum coordinates per line to get to endpoints
    tiles = [np.array([step[d] for d in re.findall(pat, line)], dtype=np.int8).sum(axis=0) for line in f]
flip, count = np.unique(tiles, axis=0, return_counts=True)  # "histogram" of tile endpoints
flip = flip[count & 1 == 1]                # keep the tiles which are flipped an odd number of times
print(len(flip))

# Part 2
flip -= np.amin(flip, axis=0)              # shift minimum coordinate to 0
dim = np.amax(flip, axis=0) + 1            # coordinate ranges
state = np.zeros(dim, dtype=np.uint8)      # grid of shape dim
state[tuple(flip.T)] = 1                   # mark flipped tiles on the grid
kernel = np.zeros((3, 3), dtype=np.uint8)  # centre cell and all 8 neighbours on square grid
i = (np.array(list(step.values()))).astype(np.uint8) + 1  # coordinates of neighbours
kernel[tuple(i.T)] = 1                     # pattern to select 6 neighbouring cells on hex grid
for _ in range(100):                       # number of generations
    nb = convolve2d(state, kernel)         # count neighbours
    state = np.pad(state, ((1,1),), mode='constant') & (nb == 1) | (nb == 2)  # evolve
print(np.sum(state))
