def knot(lengths, rounds=1):
    hlen = 256
    hash = list(range(hlen))
    pos = skip = 0
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
    return hash

# Part 1
lengths = list(map(int, open("../aocinput/2017-10-input.txt").read().split(",")))
a = knot(lengths)
print(a[0] * a[1])

# Part 2
from functools import reduce
lengths = list(map(ord, open("../aocinput/2017-10-input.txt").read().strip())) + [17, 31, 73, 47, 23]
sparse = knot(lengths, 64)
for i in range(0, len(sparse), 16):
    print('%02x' % reduce(lambda x, y: x ^ y, sparse[i:i + 16]), end='')
print()
