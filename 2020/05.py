hi = 0
seats = []
with open('../aocinput/2020-05-input.txt') as f:
	for str in (line.rstrip('\n') for line in f):
		id = int(''.join(['1' if c == 'B' or c == 'R' else '0' for c in str]), 2)
		hi = max(hi, id)
		seats.append(id)

# Part 1
print(hi)

# Part 2
seats.sort()
for ix, id in enumerate(seats, start=seats[0]):
	if ix != id:
		print(id - 1)
		break
