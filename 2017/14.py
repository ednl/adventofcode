import numpy as np
from functools import reduce

# From day 10
def knot(lenstr, rounds=64):
    hlen = 256
    hash = list(range(hlen))
    pos = skip = 0
    lengths = list(map(ord, lenstr)) + [17, 31, 73, 47, 23]
    for _ in range(rounds):
        for length in lengths:
            i = pos
            j = pos + length - 1
            while i < j:
                p = i % hlen
                q = j % hlen
                hash[p], hash[q] = hash[q], hash[p]
                i += 1
                j -= 1
            pos += length + skip
            skip += 1
    dense = ""
    for i in range(0, hlen, 16):
        dense += '{:08b}'.format(reduce(lambda x, y: x ^ y, hash[i:i + 16]))
    return np.array(list(dense)).astype(np.uint8)

disk = np.zeros((128, 128), dtype=np.uint8)
key = 'hxtvlmkl-'
for i in range(128):
    s = key + str(i)
    disk[i, :] = knot(s)
print(disk.sum())

def dfs(i, j, r):
    stack = set()
    stack.add((i, j))
    while len(stack):
        i, j = stack.pop()
        disk[i, j] = r
        if i > 0 and disk[i - 1, j] == 1:
            stack.add((i - 1, j))
        if j < 127 and disk[i, j + 1] == 1:
            stack.add((i, j + 1))
        if j > 0 and disk[i, j - 1] == 1:
            stack.add((i, j - 1))
        if i < 127 and disk[i + 1, j] == 1:
            stack.add((i + 1, j))

region = 1
for i in range(128):
    for j in range(128):
        if disk[i, j] == 1:
            region += 1
            print(i, j, region)
            dfs(i, j, region)
print(region - 1)
