seats = []
with open('../aocinput/2020-05-input.txt') as f:
    for s in (line.rstrip('\n') for line in f):
        # Boarding passes are really just 10-bit binary numbers.
        # A pass = 7x F/B, 3x L/R where B/R denote upper half (1)
        # and F/L lower half (0) of every partition.
        seats.append(int(''.join(['1' if c == 'B' or c == 'R' else '0' for c in s]), 2))

# Part 1
seats.sort()      # could use max() but need sort for part 2 anyway
print(seats[-1])  # 861

# Part 2
for ix, id in enumerate(seats, start=seats[0]):  # index and ID should be identical
    if ix != id:       # first anomaly = one after empty seat
        print(id - 1)  # 633
        break
