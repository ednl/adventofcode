import numpy as np
from scipy.signal import convolve2d

data = []
with open('../aocinput/2020-24-input.txt') as f:
    i = 0
    for line in f:
        data.append([])
        j = 0
        s = line.strip()
        while j < len(s):
            ch = s[j]
            if ch == 'e' or ch == 'w':
                j += 1
            else:
                ch += s[j + 1]
                j += 2
            data[i].append(ch)
        i += 1

# Determine grid size
maxdist = max(map(len, data))  # max number of steps per line
dim = 2 * (maxdist + 1) + 1  # both directions + border, 1 in the middle
mid = dim // 2  # midpoint
grid = np.zeros((dim, dim), dtype=np.uint8)

# Part 1
# Steps from (row,col) for even row, odd row
step = {
    'e' : [[ 0,  1], [ 0,  1]],
    'w' : [[ 0, -1], [ 0, -1]],
    'ne': [[-1,  0], [-1,  1]],
    'nw': [[-1, -1], [-1,  0]],
    'se': [[ 1,  0], [ 1,  1]],
    'sw': [[ 1, -1], [ 1,  0]]
}

for path in data:
    r = c = mid
    for nav in path:
        dr, dc = step[nav][r & 1]
        r += dr
        c += dc
    grid[r, c] ^= 1

print(np.sum(grid))

# Part 2
# Kernels for even,odd rows
kernel = np.array([[
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0]
], [
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 1]
]], dtype=np.uint8)

# Grid starts with odd numbered row! (apparently..)
for _ in range(100):
    nb0 = convolve2d(grid, kernel[1])
    nb1 = convolve2d(grid, kernel[0])
    grid = np.pad(grid, ((1,1),), mode='constant')
    grid[::2] = grid[::2] & (nb0[::2] == 1) | (nb0[::2] == 2)
    grid[1::2] = grid[1::2] & (nb1[1::2] == 1) | (nb1[1::2] == 2)

print(np.sum(grid))
