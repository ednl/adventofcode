target = 325489

# Part 1
x = y = r = 0
v = 1
while v < target:
    if x == r and y == -r:
        x += 1
        r += 1
    elif x == r and y < r:
        y += 1
    elif y == r and x > -r:
        x -= 1
    elif x == -r and y > -r:
        y -= 1
    elif y == -r and x < r:
        x += 1
    v += 1
print(abs(x) + abs(y))

# Part 2
import numpy as np
from numpy.core.numeric import convolve
x = y = r = 0
v = s = 1
a = np.zeros((s, s), dtype=np.uint32)
while v <= target:
    a[r + y, r + x] = v
    if x == r and y == r:
        a = np.pad(a, 1)
        x += 1
        r += 1
        s += 2
    elif x == r and y > -r:
        y -= 1
    elif y == -r and x > -r:
        x -= 1
    elif x == -r and y < r:
        y += 1
    else: # elif y == r and x < r:
        x += 1
    v = 0
    for i in range(r + y - 1, r + y + 2):
        if i >= 0 and i < s:
            for j in range(r + x - 1, r + x + 2):
                if j >= 0 and j < s:
                    v += a[i, j]
print(v)
