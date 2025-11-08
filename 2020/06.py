n1 = n2 = 0
a1 = a2 = set()
first = True

with open('../aocinput/2020-06-input.txt') as f:
    for str in (line.strip() for line in f):
        if str == '':
            n1 += len(a1)
            n2 += len(a2)
            first = True
        elif first:
            a1 = a2 = set(str)
            first = False
        else:
            a1 = a1.union(str)
            a2 = a2.intersection(str)

if not first:
    n1 += len(a1)
    n2 += len(a2)

print(n1)
print(n2)
