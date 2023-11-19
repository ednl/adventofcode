import re

with open('../aocinput/2015-08-input.txt') as f:
    data = [line.rstrip('\n') for line in f]

n0 = n1 = n2 = 0
for s in data:
    n0 += len(s)

    # Part 1
    s1 = s
    s1 = re.sub(r'^"|"$', r'', s1)
    s1 = re.sub(r'\\\\|\\"|\\x[0-9a-f]{2}', r'-', s1)
    n1 += len(s1)

    # Part 2
    s2 = s
    s2 = re.sub(r'^"|"$', r'---', s2)
    s2 = re.sub(r'\\\\|\\"', r'----', s2)
    s2 = re.sub(r'\\x[0-9a-f]{2}', r'-----', s2)
    n2 += len(s2)

print(n0 - n1)  # 1350
print(n2 - n0)  # 2085
