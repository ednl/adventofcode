import re
from itertools import permutations

rule = re.compile(r'^(\w+) would (gain|lose) (\d+) happiness units by sitting next to (\w+)\.$')
person = []
adj = []

with open('../aocinput/2015-13-input.txt') as f:
    for line in f:
        if m := rule.match(line.strip()):
            if m[1] not in person:
                person.append(m[1])
            if m[4] not in person:
                person.append(m[4])
            i = person.index(m[1])
            j = person.index(m[4])
            k = int(m[3])
            if m[2] == 'lose':
                k = -k
            for _ in range(i - len(adj) + 1):
                adj.append([0] * len(person))
            adj[i] += [0] * (j - len(adj[i]) + 1)
            adj[i][j] = k

max_k = 0
mem_d = 0
for p in permutations(range(len(person))):
    k = adj[p[0]][p[-1]] + adj[p[-1]][p[0]]
    min_d = k
    for i in range(len(person) - 1):
        d = adj[p[i]][p[i + 1]] + adj[p[i + 1]][p[i]]
        if d < min_d:
            min_d = d
        k += d
    if k > max_k:
        max_k = k
        mem_d = min_d
print(max_k, max_k - mem_d)
