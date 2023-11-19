# As /u/mebeim writes at
# https://www.reddit.com/r/adventofcode/comments/kjtg7y/2020_day_25_solutions/ggz4bpl/
# the problem can be rephased in terms of the Diffie-Hellman key exchange algorithm:
# "Given the public base g = 7, the public modulus p = 20201227, and the two public keys A and B
# (obtained as A = g^a mod p, B = g^b mod p), calculate the secret key s = A^b mod p = B^a mod p."
# Ref. https://en.wikipedia.org/wiki/Diffie-Hellman_key_exchange

# My version: pick the earliest matching key (p or q) in the loop,
# then use the loop count (e) with the other key (q or p) to
# calculate the secret key with modular exponentiation as in
# https://en.wikipedia.org/wiki/Modular_exponentiation
def secret1(p, q):
    e, k, b, m = 0, 1, 7, 20201227
    while k != p and k != q:
        k = k * b % m
        e += 1
    b = p if k == q else q
    return pow(b, e, m)

# Version by /u/wfxr looks neat, from
# https://www.reddit.com/r/adventofcode/comments/kjtg7y/2020_day_25_solutions/ggz4pxw/
# but is slow for p and q in the same order as in my input.
def secret2(p, q):
    r, k, b, m = 1, 1, 7, 20201227
    while k != p:
        k = k * b % m
        r = r * q % m
    return r

pubkeys = 15113849, 4206373
print(secret1(*pubkeys))
# print(secret2(*pubkeys))
