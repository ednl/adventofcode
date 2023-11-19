from math import prod

# Reverse a 10-bit binary number
def rev(n):
    s = '{0:010b}'.format(n)
    return int(s[::-1], 2)

# Original: t,r,b,l (=top,right,bottom,left)
# Rotations: r,-b,l,-t | -b,-l,-t,-r | -l,t,-r,b
def rotl(e):
    return [e[1], rev(e[2]), e[3], rev(e[0])]

# Flipped horizontally: -t,l,-b,r
# Rotations: l,b,r,t | b,-r,t,-l | -r,-t,-l,-b
def fhor(e):
    return [rev(e[0]), e[3], rev(e[2]), e[1]]

edges = {}
with open('../aocinput/2020-20-input.txt') as f:
    tiledata = f.read().replace('#', '1').replace('.', '0').strip().split('\n\n')
    for t in tiledata:
        s = t.strip().split('\n')
        # Edge bit patterns [t,r,b,l] (all clockwise)
        bits = [s[1], ''.join((r[-1] for r in s[1:])), s[-1][::-1], ''.join((r[0] for r in s[1:][::-1]))]
        edges = [int(x, 2) for x in bits]
        tiles.append({
            'id': int(s[0][5:-1]),
            'im': [x[1:-1] for x in s[2:-1]],
            'e': edges,
            's': set(edges) | set([int(x[::-1], 2) for x in bits]),
            'nb': 0,
            'link': []
        })

def invert(val, len=10):
    return int(''.join(('{0:0', str(len), 'b}')).format(val)[::-1], 2)

def rotateright(tile):
    tile['im'] = [''.join((r[c] for r in tile['im'][::-1])) for c in range(len(tile['im'][0]))]
    tile['e'] = [tile['e'][-1]] + tile['e'][:-1]

def rotateleft(tile):
    tile['im'] = [''.join((r[-(c+1)] for r in tile['im'])) for c in range(len(tile['im'][0]))]
    tile['e'] = tile['e'][1:] + [tile['e'][0]]

def rotate180(tile):
    tile['im'] = [r[::-1] for r in tile['im'][::-1]]
    tile['e'] = tile['e'][2:] + tile['e'][:2]

# Part 1
p = 1
for i1 in range(len(tiles) - 1):
    t1 = tiles[i1]
    for i2 in range(i1 + 1, len(tiles)):
        t2 = tiles[i2]
        inter = t1['s'] & t2['s']
        if len(inter):
            comm = inter.pop()
            j1 = t1['edges'].index(comm)
            j2 = t2['edges'].index(comm)
            t1['link'].append((j1, j2, i2))
            t2['link'].append((j2, j1, i1))
            t1['nb'] += 1
            t2['nb'] += 1
            if t1['nb'] == 4:
                break
    t1['link'] = sorted(t1['link'])
    del t1['s']
    if t1['nb'] == 2:
        p *= t1['id']
tiles[-1]['link'] = sorted(tiles[-1]['link'])
del tiles[-1]['s']

print(p)

# Part 2
print(tiles[0])
print(tiles[-1])
