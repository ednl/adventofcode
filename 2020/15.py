from array import array

def play(game: tuple, turns: int) -> int:
    if turns < 1:
        return None
    if len(game) == 0:
        game = [0]
    if turns <= len(game):
        return game[turns - 1]
    lastnum = game[-1]
    # For 'I', seen.itemsize should be >= 4, otherwise use 'L'
    seen = array('I', [0]) * turns
    for i in range(1, len(game)):
        seen[game[i - 1]] = i
    for i in range(len(game), turns):
        j = seen[lastnum]
        seen[lastnum] = i
        lastnum = 0 if j == 0 else i - j
    return lastnum

# Van Eck sequence
# https://www.rosettacode.org/wiki/Van_Eck_sequence
print('Van Eck:', [play([], i) for i in range(1, 21)])

# Test input
# https://adventofcode.com/2020/day/15
print('Test:')
from itertools import permutations
for game in permutations((1, 2, 3)):
    print(game, 'x 2020 =', play(game, 2020))

# Part 1
game = (0, 3, 6)
turns = 2020
print('Part 1:', game, 'x', turns, '=', play(game, turns))

# Part 2
game = (13, 16, 0, 12, 15, 1)
turns = 30_000_000
print('Part 2:', game, 'x', turns, '=', play(game, turns))
