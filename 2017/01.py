import numpy as np

# Read file as unsigned char, convert digits to numerical value, drop EOL
a = (np.fromfile("../aocinput/2017-01-input.txt", dtype=np.uint8) - ord('0'))[:-1]

# Circular permutation to the right by 1
b = np.roll(a, 1)

# Select each element of b which is equal to the corresponding element in a
# i.e. every element in a which is equal to its previous element, then sum
print(b[b == a].sum())

# Circular permutation by half the array length
c = np.roll(a, a.size // 2)
print(c[c == a].sum())
