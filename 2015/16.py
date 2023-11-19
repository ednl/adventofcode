id = {
    'children': 3,
    'cats': 7,
    'samoyeds': 2,
    'pomeranians': 3,
    'akitas': 0,
    'vizslas': 0,
    'goldfish': 5,
    'trees': 3,
    'cars': 2,
    'perfumes': 1
}
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

# Part 1
good = set(range(len(sue)))
for k, v in id.items():
    bad = set()
    for i in good:
        if k in sue[i]:
            if sue[i][k] != v:
                bad.add(i)
    good -= bad
print(good.pop() + 1 if len(good) == 1 else good)

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
print(good.pop() + 1 if len(good) == 1 else good)
