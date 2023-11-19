from hashlib import md5
data = b'iwrupvqb'

def zero(i, n):
    z = '0' * n
    while True:
        m = md5(data + str(i).encode())
        if m.hexdigest()[:n] == z:
            break
        i += 1
    return i

# Part 1
p = zero(0, 5)
print(p)

# Part 2
print(zero(p, 6))
