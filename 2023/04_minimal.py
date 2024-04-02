import numpy as np
win = np.loadtxt("../aocinput/2023-04-input.txt", dtype=np.uint8, usecols=range(2,12))
our = np.loadtxt("../aocinput/2023-04-input.txt", dtype=np.uint8, usecols=range(13,38))
cards = win.shape[0]
copies = np.ones(cards, dtype=np.uint32)
score = 0
for i in range(cards):
    match = np.intersect1d(win[i], our[i], assume_unique=True).shape[0]
    if match > 0:
        score += 1 << (match - 1)
        if i + 1 < cards:
            copies[i + 1:min(i + 1 + match, cards)] += copies[i]
print(score, np.sum(copies))  # 24733 5422730
