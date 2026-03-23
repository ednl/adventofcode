from math import sqrt
from time import monotonic_ns
t0 = monotonic_ns()

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

print(composite)  # 917

t1 = monotonic_ns()
print('Time: {:0.3f} ms'.format((t1 - t0) / 1_000_000))
