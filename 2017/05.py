data = list(map(int, open("../aocinput/2017-05-input.txt")))
size = len(data)

# Part 1
vm = data.copy()
ip = tick = 0
while (ip >= 0 and ip < size):
    jr = vm[ip]
    vm[ip] += 1
    ip += jr
    tick += 1
print(tick)

# Part 2
vm = data
ip = tick = 0
while (ip >= 0 and ip < size):
    jr = vm[ip]
    vm[ip] += 1 if jr < 3 else -1
    ip += jr
    tick += 1
print(tick)
