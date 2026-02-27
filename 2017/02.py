import numpy as np

# Read text as array
a = np.loadtxt("../aocinput/2017-02-input.txt", dtype=np.uint32)
a.sort(axis=1)

# Part 1
print((a[:,-1] - a[:,0]).sum())  # 34925

# Part 2
type Vector = list[int]
def finddiv(a: Vector) -> int:
    for i, y in enumerate(a[:-1]):
        for x in a[i + 1:]:
            z = divmod(x, y)
            if z[1] == 0:
                return z[0]
    return 0

k = 0
for b in a:
    k += finddiv(b)
print(k)  # 221
