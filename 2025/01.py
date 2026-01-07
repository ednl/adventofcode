# Timing
from time import monotonic_ns
t0 = monotonic_ns()

# Read and parse input file
with open('../aocinput/2025-01-input.txt') as f:
    turns = [{'dir': -1 if line[0] == 'L' else 1, 'count': int(line[1:])} for line in f]

# Initial values
dial = 50
sum1 = 0
sum2 = 0
debug = True

# For every line in the input file
for index, turn in enumerate(turns):

    # One click at a time
    for _ in range(turn['count']):
        dial += turn['dir']  # +1 or -1
        dial %= 100  # Python '%' operator result: dial>=0 and dial<100

        # Part 2: count zeros during turning
        if dial == 0:
            sum2 += 1

    # Part 1: count zeros after turning
    if dial == 0:
        sum1 += 1

    # Debug output: intermediate values per line of the input file
    if debug:
        print(f'{index:4}: {'L' if turn['dir'] == -1 else 'R'}{turn['count']:<3} => {sum1},{sum2}')
        if turn['count'] >= 100:
            debug = False  # stop debug output after first big turn
            print()

# Solution
print(sum1, sum2)

# Timing
t1 = monotonic_ns()
print('Time: {:5.1f} ms'.format((t1 - t0) / 1000000))
