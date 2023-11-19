from itertools import combinations

weapon = [
    {'g':   8, 'd': 4, 'a': 0},
    {'g':  10, 'd': 5, 'a': 0},
    {'g':  25, 'd': 6, 'a': 0},
    {'g':  40, 'd': 7, 'a': 0},
    {'g':  74, 'd': 8, 'a': 0}
]
armor = [
    {'g':   0, 'd': 0, 'a': 0},
    {'g':  13, 'd': 0, 'a': 1},
    {'g':  31, 'd': 0, 'a': 2},
    {'g':  53, 'd': 0, 'a': 3},
    {'g':  75, 'd': 0, 'a': 4},
    {'g': 102, 'd': 0, 'a': 5}
]
ring = [
    {'g':  25, 'd': 1, 'a': 0},
    {'g':  50, 'd': 2, 'a': 0},
    {'g': 100, 'd': 3, 'a': 0},
    {'g':  20, 'd': 0, 'a': 1},
    {'g':  40, 'd': 0, 'a': 2},
    {'g':  80, 'd': 0, 'a': 3}
]

bd = 8
ba = 1
gmin = float('+inf')
gmax = float('-inf')
for w in weapon:
    g1 = w['g']
    pd1 = w['d']
    for a in armor:
        g2 = g1 + a['g']
        pa1 = a['a']
        for n in range(3):
            for c in combinations(ring, n):
                g3 = g2
                pd2 = pd1
                pa2 = pa1
                for r in c:
                    g3 += r['g']
                    pd2 += r['d']
                    pa2 += r['a']
                dbh = max(1, pd2 - ba)
                dph = max(1, bd - pa2)
                bh = 104
                ph = 100
                while True:
                    bh -= dbh
                    if bh <= 0:
                        break
                    ph -= dph
                    if ph <= 0:
                        break
                if bh <= 0 and g3 < gmin:
                    gmin = g3
                if ph <= 0 and g3 > gmax:
                    gmax = g3
# correct: 78, 148
print(gmin, gmax)
