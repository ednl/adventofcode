with open('../aocinput/2015-03-input.txt') as f:
    data = f.readline().rstrip('\n')

v = {'>': (1,0), '<': (-1,0), '^': (0,1), 'v': (0,-1)}

def pres(direc):
    s = set()
    x = y = 0
    s.add((x,y))
    for c in direc:
        dx, dy = v[c]
        x += dx
        y += dy
        s.add((x,y))
    return s

# Part 1
print(len(pres(data)))

# Part 2
print(len(pres(data[::2]).union(pres(data[1::2]))))
