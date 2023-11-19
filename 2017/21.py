import numpy as np
from math import sqrt

def str2bit(s):
    return s.strip().replace('/', '').replace('.', '0').replace('#', '1')

def str2val(s):
    return int(str2bit(s), 2)

def str2arr(s):
    return (np.array([list(row) for row in s.strip().split('/')]) == '#').astype(np.uint8)

def arr2val(a):
    return int(''.join(map(str, a.flatten())), 2)

def permvals(a):
    p = set()
    p.add(arr2val(a))
    a = a.T
    p.add(arr2val(a))
    a = np.flipud(a)
    p.add(arr2val(a))
    a = a.T
    p.add(arr2val(a))
    a = np.flipud(a)
    p.add(arr2val(a))
    a = a.T
    p.add(arr2val(a))
    a = np.flipud(a)
    p.add(arr2val(a))
    a = a.T
    p.add(arr2val(a))
    return list(p)

rule = { 2: {}, 3: {} }
with open('21.txt') as f:
    for line in f:
        a, b = line.split(' => ')
        i = 2 if len(a) == 5 else 3
        p = permvals(str2arr(a))  # search patterns
        r = str2val(b)            # replacement
        for j in p:
            rule[i][j] = r

def partition(bitstr, area, size, chunk):
    step = size * chunk  # index step of 2 or 3 rows
    p = []
    for i in range(0, area, step):   # row index per 2 or 3 rows
        for j in range(0, size, chunk):  # col index per 2 or 3 cols
            s = ''
            for k in range(0, step, size):  # extra row index for 2 or 3 conseq. rows
                n = i + j + k
                s += bitstr[n:n + chunk]
            p.append(int(s, 2))
    return p

def val2bit(val, partarea):
    s = bin(val)[2:]
    return '0' * (partarea - len(s)) + s

def evolve(bitstr):
    area = len(bitstr)
    size = int(sqrt(area))
    chunk = 2 if size % 2 == 0 else 3
    parts = partition(bitstr, area, size, chunk)
    transform = list(map(lambda x: rule[chunk][x], parts))
    partperrow = size // chunk
    parts = partperrow * partperrow
    chunk += 1
    size = partperrow * chunk
    area = size * size
    partarea = chunk * chunk
    a = list(map(lambda x: val2bit(x, partarea), transform))
    s = ''
    for i in range(0, parts, partperrow):
        for k in range(0, partarea, chunk):
            for j in range(partperrow):
                s += a[i + j][k:k + chunk]
    return s

image = '.#./..#/###'
art = str2bit(image)
print(0, sum(map(int, list(art))))
for i in range(1, 19):
    art = evolve(art)
    print(i, sum(map(int, list(art))))
# part 1: 179
# part 2: 2766750
