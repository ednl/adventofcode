reg = {}
rmax = 0
with open("../aocinput/2017-08-input.txt") as f:
    for line in f:
        m = line.split()
        reg.setdefault(m[0], 0)
        reg.setdefault(m[4], 0)
        m[0] = "reg['" + m[0] + "']"
        m[4] = "reg['" + m[4] + "']"
        m[1] = "+=" if m[1] == "inc" else "-="
        exec(" ".join(m) + " else 0")
        if eval(m[0]) > rmax:
            rmax = eval(m[0])
m = 0
for v in reg.values():
    if v > m:
        m = v
print(m, rmax)
