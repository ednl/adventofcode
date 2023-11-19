import numpy as np

def state(a):
    return "".join(list(map(chr, a + 32)))

a = np.loadtxt("../aocinput/2017-06-input.txt", dtype=np.uint8)
banks = a.size

seen = set()
hist = list()
hash = state(a)
count = 0

while hash not in seen:
    count += 1
    seen.add(hash)
    hist.append(hash)

    val = np.amax(a)
    key = np.argmax(a)
    a[key] = 0
    q, r = divmod(val, banks)

    a += q
    next = key - banks + 1
    for i in range(next, next + r):
        a[i] += 1

    hash = state(a)

# Part 1
print(count)

# Part 2
print(count - hist.index(hash))
