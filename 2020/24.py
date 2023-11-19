from copy import deepcopy

#  r,c | 0 1 2 3 4 5 6
# -----+--------------
#  0   | x x x x x x x
#  1   |  x x x x x x x
#  2   | x x x x x x x
#  3   |  x x x x x x x
#  4   | x x x x x x x

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
generations = 100  # for part 2
maxdist = max(map(len, data))  # max number of steps per line, might be in one direction
dim = 2 * (maxdist + generations) + 3  # distance can grow 1 for every generation 
mid = dim // 2  # midpoint

g1 = []
for i in range(dim):
    g1.append([0] * dim)

# Part 1
# Steps for (row,col) from even row, odd row
step = {
    'e' : (( 0,  1), ( 0,  1)),
    'w' : (( 0, -1), ( 0, -1)),
    'ne': ((-1,  0), (-1,  1)),
    'nw': ((-1, -1), (-1,  0)),
    'se': (( 1,  0), ( 1,  1)),
    'sw': (( 1, -1), ( 1,  0))
}

def next(r, c, nav):
    dr, dc = step[nav][r % 2]
    return r + dr, c + dc

for path in data:
    r = c = mid
    for nav in path:  # follow the complete path
        r, c = next(r, c, nav)
    g1[r][c] ^= 1  # flip the tile

print(sum(map(sum, g1)))

# Part 2
# import numpy as np
# from scipy.signal import convolve2d

# Kernels for even,odd rows
# kernel = np.array([[
#     [1, 1, 0],
#     [1, 0, 1],
#     [1, 1, 0]
# ], [
#     [0, 1, 1],
#     [1, 0, 1],
#     [0, 1, 1]
# ]])

# state = np.array(g1)
# for _ in range(generations):
#     nb0 = convolve2d(state, kernel[0], mode='same')
#     nb1 = convolve2d(state, kernel[1], mode='same')
#     state[::2] = state[::2] & (nb0[::2] == 1) | (nb0[::2] == 2)
#     state[1::2] = state[1::2] & (nb1[1::2] == 1) | (nb1[1::2] == 2)

# print(np.sum(state))

# Kernels for even,odd rows
kernel = [[
    [1, 1, 0],
    [1, 0, 1],
    [1, 1, 0]
], [
    [0, 1, 1],
    [1, 0, 1],
    [0, 1, 1]
]]

def neighbours(g, k, r, c):
    n = 0
    r -= 1
    c -= 1
    for i in range(3):
        kr = k[i]
        gr = g[r + i]
        for j in range(3):
            n += kr[j] * gr[c + j]
    return n

g2 = deepcopy(g1)
lim0 = mid - maxdist
lim1 = mid + maxdist + 1
for _ in range(generations):
    for r in range(lim0, lim1):
        k = kernel[r % 2]
        for c in range(lim0, lim1):
            nb = neighbours(g1, k, r, c)
            g2[r][c] = 1 if nb == 2 or (nb == 1 and g1[r][c] == 1) else 0
    g1, g2 = g2, g1
    lim0 -= 1
    lim1 += 1

print(sum(map(sum, g1)))
