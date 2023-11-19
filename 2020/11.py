from copy import deepcopy

c2n = {'.': -1, 'L': 0, '#': 1}
n2c = {v: k for k, v in c2n.items()}

data = []
with open('../aocinput/2020-11-input.txt') as file:
	for line in file:
		data.append([c2n[c] for c in line.strip()])

w, h = len(data[0]), len(data)

def neighbours(area, row, col, thresh, part):
	n = 0
	for i in range(-1, 2):
		for j in range(-1, 2):
			if i or j:
				r = row + i
				c = col + j
				while r >= 0 and r < h and c >= 0 and c < w:
					if area[r][c] != -1 or part == 1:
						if area[r][c] == 1:
							n += 1
							if n == thresh:
								return n
						break
					r += i
					c += j
	return n

def evolve(a1, a2, part):
	changed = False
	for r in range(h):
		for c in range(w):
			if a1[r][c] == 0:
				if neighbours(a1, r, c, 1, part) == 0:
					a2[r][c] = 1
					changed = True
				else:
					a2[r][c] = 0
			elif a1[r][c] == 1:
				if neighbours(a1, r, c, 3 + part, part) >= 3 + part:
					a2[r][c] = 0
					changed = True
				else:
					a2[r][c] = 1
	return changed

def occupied(area):
	n = 0
	for row in area:
		for seat in row:
			if seat == 1:
				n += 1
	return n

for p in range(1, 3):
	a = deepcopy(data)
	b = deepcopy(data)
	while evolve(a, b, p):
		a, b = b, a
	print(occupied(a))
