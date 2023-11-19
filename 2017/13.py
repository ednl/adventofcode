layer = {}
with open("../aocinput/2017-13-input.txt") as f:
    for line in f:
        a = list(map(int, line.strip().split(": ")))
        layer[a[0]] = a[1]

# Part 1
severity = 0
for d, r in layer.items():
    if d % (2 * (r - 1)) == 0:
        severity += d * r
print(severity)

# Part 2
delay = 0
caught = True
while caught:
    delay += 1
    for d, r in layer.items():
        if caught := (((d + delay) % (2 * (r - 1))) == 0):
            break
print(delay)
