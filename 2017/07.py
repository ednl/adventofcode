import re
r0 = re.compile('^([a-z]+) \((\d+)\)(?: -> ([a-z, ]+))?')

# Part 1
allprogs = set()
hasparent = set()
with open("../aocinput/2017-07-input.txt") as f:
    for line in f:
        if m := r0.match(line):
            allprogs.add(m[1])
            if m[3] is not None:
                a = set(m[3].split(", "))
                allprogs |= a
                hasparent |= a
root = (allprogs - hasparent).pop()
print(root)

# Part 2
prog = {}
with open("../aocinput/2017-07-input.txt") as f:
    for line in f:
        if m := r0.match(line):
            parent = m[1]
            weight = int(m[2])
            if m[3] is None:
                children = []
            else:
                children = m[3].split(", ")
                for child in children:
                    if child in prog:
                        prog[child]['parent'] = parent
                    else:
                        prog[child] = { 'parent': parent }

            if parent in prog:
                prog[parent]['weight'] = weight
                prog[parent]['children'] = children
            else:
                prog[parent] = { 'weight': weight, 'children': children }

def weight(name):
    global prog
    if name not in prog:
        return 0
    w = prog[name].setdefault('total', 0)
    if w != 0:
        return w
    w = prog[name].setdefault('weight', 0)
    c = prog[name].setdefault('children', [])
    for n in c:
        w += weight(n)
    prog[name]['total'] = w
    return w

def findbalanced(name, target):
    global prog
    if name not in prog:
        return None
    c = prog[name].setdefault('children', [])
    if len(c) == 0:
        w = target - weight(name) + prog[name]['weight']
        return name + " = " + str(prog[name]['weight']) + " => " + str(w)
    w0 = w1 = w2 = 0
    n0 = n1 = n2 = None
    for n in c:
        n0 = n1
        n1 = n2
        n2 = n
        w0 = w1
        w1 = w2
        w2 = weight(n)
        if w0 != 0:
            if w0 == w1 and w0 != w2:
                return findbalanced(n2, w0)
            elif w0 == w2 and w0 != w1:
                return findbalanced(n1, w0)
            elif w1 == w2 and w1 != w0:
                return findbalanced(n0, w1)
    w = target - weight(name) + prog[name]['weight']
    return name + " = " + str(prog[name]['weight']) + " => " + str(w)

print(findbalanced(root, 0))
