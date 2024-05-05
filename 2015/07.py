import re
regrule = re.compile(r'^(([a-z0-9]+) )?(([A-Z]+) )?([a-z0-9]+) -> ([a-z]+)\n$')

fun = {
    'AND'   : lambda x, y: x & y,
    'OR'    : lambda x, y: x | y,
    'NOT'   : lambda x: ~x,
    'LSHIFT': lambda x, y: x << y,
    'RSHIFT': lambda x, y: x >> y,
    'PATCH' : lambda x: x}

h = {}  # h = connection diagram: for every output wire list its input wires and gate function
with open('../aocinput/2015-07-input.txt') as fp:
    for line in fp:
        m = regrule.match(line)
        if m:
            f = fun[m.group(4) if m.group(4) is not None else 'PATCH']
            u = [int(x) if x.isnumeric() else x for x in m.group(2, 5) if x is not None]
            y = m.group(6)           # y = output wire
            h[y] = {'f': f, 'u': u}  # u = input wires, connected via gate function f

def getval(id):
    if type(id) is int:                  # direct value?
        return id
    if id in val:                        # already calculated?
        return val[id]
    inp = list(map(getval, h[id]['u']))  # evaluate input wires u
    for u, v in zip(h[id]['u'], inp):    # pair input wires with values
        val[u] = v                       # save input values
    return h[id]['f'](*inp)              # return output value

# Part 1
val = {}         # cache
a = getval('a')  # remember value for part 2
print(a)         # 46065

# Part 2
val = {'b': a}      # init with value from part 1
print(getval('a'))  # 14134
