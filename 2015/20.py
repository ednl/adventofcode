from itertools import combinations
from math import prod

primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]
target = 33100000

def presents(n):
    a = []
    for p in primes:
        if p > n:
            break
        while n % p == 0:
            a.append(p)
            n //= p
    f = 101
    while f * f < n:
        if n % f == 0:
            a.append(f)
            n //= f
        else:
            f += 2
    if n != 1:
        a.append(n)
    p = set([1])
    for r in range(1, len(a) + 1):
        for c in combinations(a, r):
            p.add(prod(c))
    return sum(p)

def morethan(t):
    t //= 10
    n = t // 5
    q = 0
    while q < t:
        n += 1
        q = presents(n)
    return n

# 776160
# print(morethan(target))

# 1: 1,2,3,...,50
# 2: 2,4,6,...,100
# 3: 3,6,9,...,150
# 50: 50,100,150,...,2500
# 100: 100,200,300,...,5000

def pres2(n):
    p = 0
    f = (n - 1) // 50 + 1
    while f <= n:
        if n % f == 0:
            p += f
        f += 1
    return p * 11

n = 776160
# 782843 too low
# target//36: 919800 too high
q = 0
while q < target:
    n += 1
    q = pres2(n)
    print(n, target - q)
