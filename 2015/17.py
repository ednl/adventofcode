from itertools import combinations

target = 150
with open('../aocinput/2015-17-input.txt') as f:
    data = sorted(map(int, list(f)))

mincount = maxcount = len(data)
for i in range(1, len(data)):
    if i < maxcount and sum(data[:i]) > target:
        maxcount = i - 1
    if i < mincount and sum(data[-1:-1  - i:-1]) >= target:
        mincount = i

n = m = firstbatch = 0
for count in range(mincount, maxcount + 1):
    for c in combinations(data, count):
        if sum(c) == target:
            n += 1
            if firstbatch == 0:
                firstbatch = count
            if count == firstbatch:
                m += 1
print(n, m)
