from time import monotonic_ns
t0 = monotonic_ns()

layer = {}
with open("../aocinput/2017-13-input.txt") as f:
    for line in f:
        a = list(map(int, line.strip().split(": ")))
        # Period of a scanner with range r is 2(r-1)
        layer[a[0]] = 2 * (a[1] - 1)

# Part 1
severity = 0
for depth, period in layer.items():
    if depth % period == 0:
        # Range of a scanner with period p is p/2+1
        severity += depth * (period // 2 + 1)
print(severity)  # 2604

# Part 2
delay = 0
caught = True
while caught:
    delay += 1
    for depth, period in layer.items():
        if caught := (((depth + delay) % period) == 0):
            break
print(delay)  # 3941460

t1 = monotonic_ns()
print('Time: {:0.3f} s'.format((t1 - t0) / 1_000_000_000))
