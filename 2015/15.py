import numpy as np
import re
num = re.compile(r'-?\d+')
with open('../aocinput/2015-15-input.txt') as f:
    ingredient = np.array(list(map(int, re.findall(num, f.read()))), dtype=np.int64).reshape((4, 5))

hiscore = 0
calscore = 0
for a in range(1, 98):
    for b in range(1, 99 - a):
        for c in range(1, 100 - a - b):
            d = 100 - a - b - c
            spoon = np.array([[a, b, c, d]], dtype=np.int64)
            cookie = np.sum(spoon.T * ingredient, axis=0)
            cookie[cookie < 0] = 0
            score = np.prod(cookie[:-1])
            if score > hiscore:
                hiscore = score
            if cookie[-1] == 500 and score > calscore:
                calscore = score

print(hiscore, calscore)
