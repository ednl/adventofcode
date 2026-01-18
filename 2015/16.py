from time import monotonic_ns
t0 = monotonic_ns()

sue = []
with open('../aocinput/2015-16-input.txt') as f:
    for line in f:
        _, has = line.strip().split(': ', maxsplit=1)
        has = has.split(', ')
        d = {}
        for h in has:
            k, v = h.split(': ', maxsplit=1)
            d[k] = int(v)
        sue.append(d)

t1 = monotonic_ns()

# From puzzle description at https://adventofcode.com/2015/day/16
id = {
    'children'   : 3,
    'cats'       : 7,
    'samoyeds'   : 2,
    'pomeranians': 3,
    'akitas'     : 0,
    'vizslas'    : 0,
    'goldfish'   : 5,
    'trees'      : 3,
    'cars'       : 2,
    'perfumes'   : 1,
}

# Part 1
good = set(range(len(sue)))
for k, v in id.items():
    bad = set()
    for i in good:
        if k in sue[i]:
            if sue[i][k] != v:
                bad.add(i)
    good -= bad
print('Part 1:', good.pop() + 1 if len(good) == 1 else good)  # 213

t2 = monotonic_ns()

# Part 2
good = set(range(len(sue)))
for k, v in id.items():
    bad = set()
    for i in good:
        if k in sue[i]:
            if k == 'cats' or k == 'trees':
                # must be greater
                if sue[i][k] <= v:
                    bad.add(i)
            elif k == 'pomeranians' or k == 'goldfish':
                # must be smaller
                if sue[i][k] >= v:
                    bad.add(i)
            else:
                if sue[i][k] != v:
                    bad.add(i)
    good -= bad
print('Part 2:', good.pop() + 1 if len(good) == 1 else good)  # 323

t3 = monotonic_ns()
print('Read+parse : {:4.0f} µs'.format((t1 - t0) / 1_000))
print('Part 1     : {:4.0f} µs'.format((t2 - t1) / 1_000))
print('Part 2     : {:4.0f} µs'.format((t3 - t2) / 1_000))
print('All        : {:4.0f} µs'.format((t3 - t0) / 1_000))
print('Part 1+2   : {:4.0f} µs'.format((t3 - t1) / 1_000))
