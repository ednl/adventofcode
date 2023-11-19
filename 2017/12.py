import numpy as np
pipe = np.zeros((2000, 2000), dtype=np.uint8).astype('?')
with open("../aocinput/2017-12-input.txt") as f:
    for line in f:
        a = line.strip().split(' <-> ')
        i = int(a[0])
        b = list(map(int, a[1].split(', ')))
        for j in b:
            pipe[i, j] = pipe[j, i] = True

def connected(i):
    return np.arange(2000)[pipe[i, :]]

def connect(i):
    pre = 0
    post = pipe[i, :].sum()
    while pre != post:
        pre = post
        pipe[i, i] = True
        for j in connected(i):
            pipe[i, :] = pipe[i, :] | pipe[j, :]
        post = pipe[i, :].sum()
    return post

# Part 1
print(connect(0))

# Part 2
for i in range(1, 2000):
    connect(i)
print(np.unique(pipe, axis=0).shape[0])
