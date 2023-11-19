from math import sqrt

magic = 65

start = magic * 100 + 100_000
stop = start + 17_000
composite = 0
for x in range(start, stop + 1, 17):
    if x % 2 == 0:
        composite += 1
        continue
    for d in range(3, int(sqrt(x)) + 1, 2):
        if x % d == 0:
            composite += 1
            break

print(composite)
