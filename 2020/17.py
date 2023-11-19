import numpy as np
from scipy.signal import convolve
from time import perf_counter as timer

with open('../aocinput/2020-17-input.txt') as f:
    data = (np.array([list(i.strip()) for i in f]) == '#').astype(np.uint8)

def cycle(init, dim, gen=6):
    state = init.reshape([1] * (dim - init.ndim) + list(init.shape))
    kernel = np.ones([3] * dim, dtype=np.uint8)
    for _ in range(gen):
        nb = convolve(state, kernel)
        state = np.pad(state, ((1,1),), mode='constant') & (nb == 4) | (nb == 3)
    return np.sum(state)

for dim in range(3, 7):
    t0 = timer()
    res = cycle(data, dim)
    t1 = timer()
    print('%2d %5d %.3f' % (dim, res, t1 - t0))
