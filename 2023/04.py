from time import monotonic_ns
t0 = monotonic_ns()

import numpy as np
t1 = monotonic_ns()

win = np.loadtxt("../aocinput/2023-04-input.txt", dtype=np.uint8, usecols=range(2,12))
our = np.loadtxt("../aocinput/2023-04-input.txt", dtype=np.uint8, usecols=range(13,38))
t2 = monotonic_ns()

cards = win.shape[0]
copies = np.ones(cards, dtype=np.uint32)

score = 0
for i in range(cards):
    match = np.intersect1d(win[i], our[i], assume_unique=True).shape[0]
    if match > 0:
        score += 1 << (match - 1)
        j = i + 1
        if j < cards:
            k = min(j + match, cards)
            copies[j:k] += copies[i]
t3 = monotonic_ns()

print(score, np.sum(copies))
t4 = monotonic_ns()

print('import  : {:6.2f} ms'.format((t1 - t0)/1000000))
print('loadtxt : {:6.2f} ms'.format((t2 - t1)/1000000))
print('logic   : {:6.2f} ms'.format((t3 - t2)/1000000))
print('print   : {:6.2f} ms'.format((t4 - t3)/1000000))
print('total   : {:6.2f} ms'.format((t4 - t0)/1000000))
