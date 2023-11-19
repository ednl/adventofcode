def duel1(a, b, c):
    k = 0
    for _ in range(c):
        a = a * 16807 % 2147483647
        b = b * 48271 % 2147483647
        k += a & 0xffff == b & 0xffff
    return k

def duel2(a, b, c):
    k = 0
    for _ in range(c):
        a = a * 16807 % 2147483647
        while a & 0x3:
            a = a * 16807 % 2147483647
        b = b * 48271 % 2147483647
        while b & 0x7:
            b = b * 48271 % 2147483647
        k += a & 0xffff == b & 0xffff
    return k

print(duel1(783, 325, 40_000_000))
print(duel2(783, 325, 5_000_000))
