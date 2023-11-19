# Linear, minimum for x = 350
def f(x):
    return abs(x - 350) + 100

# Quadratic, minimum for x = 480
def g(x):
    return x * x - 960 * x + 230600

# Assume function has one minimum between a and b
def bisect(a, b, fun):
    loop = 0
    while (a < b):
        loop += 1
        c = (a + b) // 2         # centre point, rounded down
        d = fun(c + 1) - fun(c)  # differential between x and x+1
        if d > 0:                # rising => minimum is to the left
            b = c                # new upper bound
        elif d < 0:              # falling => minimum is to the right
            a = c + 1            # new lower bound
        else:                    # constant => minimum found at both c and c+1
            a = c                #   (doesn't happen in AoC 2021 day 7)
            break
    return fun(a), loop

# Min/max from data
a = 0
b = 1878

print("Part 1:", bisect(a, b, f))
print("Part 2:", bisect(a, b, g))
