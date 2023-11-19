with open('../aocinput/2020-13-input.txt') as f:
    mytime = int(f.readline())
    buses = list(map(lambda x: 0 if x == 'x' else int(x), f.readline().rstrip('\n').split(',')))

def depart(t, n):
    return (n - t) % n

# Part 1
deps = sorted([(depart(mytime, bus), bus) for bus in filter(lambda x: x > 0, buses)])
print(deps[0][0] * deps[0][1])

# Part 2
# https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Search_by_sieving
t = 0
dt = 1
for time, bus in enumerate(buses):
    if bus:
        wait = depart(time, bus)
        while t % bus != wait:
            t += dt
        dt *= bus
print(t)
