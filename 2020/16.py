import re
from math import prod

rule = re.compile(r'^([a-z ]+): (\d+)-(\d+) or (\d+)-(\d+)$')

rules = {}
myticket = []
tickets = []

with open('../aocinput/2020-16-input.txt') as f:
    part = 1
    for line in f:
        line = line.rstrip('\n')
        if line == '':
            part += 1
            f.readline()
        elif part == 1:
            m = rule.match(line)
            if m:
                a = list(map(int, m.group(2, 3, 4, 5)))
                a[1] += 1
                a[3] += 1
                rules[m.group(1)] = [a[:2], a[2:]]
        elif part == 2:
            myticket = list(map(int, line.split(',')))
        elif part == 3:
            tickets.append(list(map(int, line.split(','))))

# Part 1
valid = set()
for r1, r2 in rules.values():
    valid = valid.union(range(r1[0], r1[1])).union(range(r2[0], r2[1]))
invalid = 0
for t in tickets:
    for n in t:
        if n not in valid:
            invalid += n
print(invalid)

# Part 2
def isvalid(ticket):
    for n in ticket:
        if n not in valid:
            return False
    return True

def inrule(rulename, n):
    for r in rules[rulename]:
        if n in range(r[0], r[1]):
            return True
    return False

impossibru = []
for i in range(len(myticket)):
    impossibru.append(set())

tickets.append(myticket)
for ticket in tickets:
    if isvalid(ticket):
        for field, num in enumerate(ticket):
            for name in rules:
                if not inrule(name, num):
                    impossibru[field].add(name)

fieldnames = set(rules.keys())
possibru = [fieldnames - i for i in impossibru]
count = sorted(list((len(v), k) for k, v in enumerate(possibru)))
for i, f1 in enumerate(count[:-1]):
    s1 = possibru[f1[1]]
    for _, f2 in count[i + 1:]:
        possibru[f2] -= s1 & possibru[f2]
fieldmapping = [f for s in possibru for f in s]
mydepartures = [n for n, f in zip(myticket, fieldmapping) if f[:9] == 'departure']
print(prod(mydepartures))
