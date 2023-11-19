from itertools import permutations
import re
rule = re.compile(r'^(\w+) to (\w+) = (\d+)$')

vert = []
adj = []

def add_vert(v):
    try:
        i = vert.index(v)
    except ValueError:
        i = len(vert)
        vert.append(v)
    return i

def set_weight(i, j, k):
    for _ in range(i - len(adj) + 1):
        adj.append([0] * len(vert))
    adj[i].extend([0] * (j - len(adj[i]) + 1))
    adj[i][j] = k

with open('../aocinput/2015-09-input.txt') as f:
    for line in f:
        line = line.rstrip('\n')
        if m := rule.match(line):
            i = add_vert(m[1])
            j = add_vert(m[2])
            k = int(m[3])
            if i != j and k > 0:
                set_weight(i, j, k)
                set_weight(j, i, k)

mindist = float('+inf')
maxdist = float('-inf')
for p in permutations(range(len(adj))):
    dist = 0
    for i in range(len(adj) - 1):
        dist += adj[p[i]][p[i + 1]]
    if dist < mindist:
        mindist = dist
    if dist > maxdist:
        maxdist = dist

print(mindist, maxdist)
