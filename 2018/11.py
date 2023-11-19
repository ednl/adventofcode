# Ref.: https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.convolve2d.html
import numpy as np
from scipy.signal import convolve2d

# Fuel cell power level as defined by the puzzle
def power(x, y, s):
    return ((x + 10) * y + s) * (x + 10) // 100 % 10 - 5

# My puzzle input
serial = 2866

# Grid of fuel cells
gridsize = 300
grid = np.zeros((gridsize, gridsize), dtype=np.int64)

# Set power level of every fuel cell
for y in range(gridsize):
    for x in range(gridsize):
        # The puzzle uses 1-based indexing
        # Also for debug printing: y=rows, x=cols
        grid[y, x] = power(x + 1, y + 1, serial)

# Investigate max fuel sum per square size >= 3x3
#   curmax = max for current square size
#   absmax = max for all square sizes
squaresize = 3  # start with 3x3 because that's part 1
curmax = absmax = 0

# Stop when no new max twice
declinestreak = 0
while declinestreak < 2 and squaresize <= gridsize:
    # Convolution kernel all ones => get sum of the kernel area
    kernel = np.ones((squaresize, squaresize), dtype=np.int64)
    # mode='valid' means do not extend beyond grid size
    squares = convolve2d(grid, kernel, mode='valid')
    # Get the index of the maximum value
    my, mx = np.unravel_index(squares.argmax(), squares.shape)
    # And the maximum value itself for this square size
    curmax = squares[my, mx]
    # Remember if best until now
    if curmax > absmax:
        absmax = curmax
        declinestreak = 0
        # Plus 1 because puzzle uses 1-based indexing
        print("%d,%d,%d" % (mx + 1, my + 1, squaresize))
    else:
        declinestreak += 1
    squaresize += 1
