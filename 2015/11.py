pwd = 'hxbxwxba'
sym = 'abcdefghjkmnpqrstuvwxyz'
base = len(sym)
size = len(pwd)

val = 0
for c in pwd:
    val = val * base + sym.index(c)

def tostr(n):
    s = ''
    while n > 0:
        s += sym[n % base]
        n //= base
    return s[::-1]

def valid(pwd):
    triplet = False
    for i in range(size - 3):
        if pwd[i:i + 3] in sym:
            triplet = True
            break
    if not triplet:
        return False
    duos = set()
    i = 0
    while i < size - 1:
        if pwd[i] == pwd[i + 1]:
            duos.add(pwd[i])
            if len(duos) == 2:
                return True
            i += 1
        i += 1
    return False

n = 0
while True:
    val += 1
    pwd = tostr(val)
    if valid(pwd):
        print(pwd, val)
        n += 1
        if n == 2:
            break
