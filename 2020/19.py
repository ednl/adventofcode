import re

rules = {}  # parsed rules from the input
cache = {}  # inferred regex for each rule
data = []   # parsed messages from the input

# Parse input
part = 1  # input has 2 parts separated by an empty line
isstr = re.compile(r'^(\d+): "([ab])"$')
isnums = re.compile(r'^(\d+): (\d+)( \d+)?( \| (\d+)( \d+)?)?$')
with open('../aocinput/2020-19-input.txt') as f:
    for line in f:
        line = line.strip()
        if len(line) == 0:
            part += 1
        elif part == 1:
            m = isstr.match(line)
            if m:
                rules[int(m.group(1))] = m.group(2)
                continue
            m = isnums.match(line)
            if m:
                rules[int(m.group(1))] = [-1 if i is None else int(i) for i in m.group(2, 3, 5, 6)]
        elif part == 2:
            data.append(line)

# Depth First Search through the rules
# with caching of intermediate results
def pattern(n):
    if n in cache:
        return cache[n]

    r = rules[n]
    if type(r) == str:
        cache[n] = r
        return r

    a = pattern(r[0])
    cache[r[0]] = a
    if r[1] >= 0:
        b = pattern(r[1])
        cache[r[1]] = b
        a += b

    if r[2] >= 0:
        c = pattern(r[2])
        cache[r[2]] = c
        if r[3] >= 0:
            d = pattern(r[3])
            cache[r[3]] = d
            c += d
        a = '(?:' + a + '|' +  c + ')'

    cache[n] = a
    return a

# Part 1
rulezero = pattern(0)
print(sum([bool(re.fullmatch(rulezero, message)) for message in data]))

# Part 2
# My rule  0  = 8 11
# New rule 8  = 42 | 42 8        = 42 | 42 42 | 42 42 42 | ...
# New rule 11 = 42 31 | 42 11 31 = 42 31 | 42 42 31 31 | 42 42 42 31 31 31 | ...
# This means that my rule 0 = 42{n} 31{m}, where n>m and m>0
# Problem: the number of repeated group matches is not returned by Python's re.match
# but looking at the rule set: every match for a single rule 42 or 31 must have the same length
# so, "n>m" can be translated as len(42+)/len(42) > len(31+)/len(31)

# print("42:", cache[42])
# print("31:", cache[31])

rulezero = '((' + cache[42] + ')+)((' + cache[31] + ')+)'
matches = 0
for message in data:
    fm = re.fullmatch(rulezero, message)
    if fm:
        if len(fm.group(1)) // len(fm.group(2)) > len(fm.group(3)) // len(fm.group(4)):
            matches += 1
print(matches)
