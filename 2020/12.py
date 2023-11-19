with open('../aocinput/2020-12-input.txt') as f:
	data = [(line[:1], int(line[1:])) for line in f]

heading = {'E': 1, 'N': 1j, 'W': -1, 'S': -1j}

def nav(way, part):
	pos = 0
	for act, val in data:
		if act == 'L' or act == 'R':
			if act == 'R':
				val = 360 - val
			while val:
				way *= 1j
				val -= 90
		elif act == 'F':
			pos += val * way
		else:
			val *= heading[act]
			if part == 1:
				pos += val
			else:
				way += val
	return int(abs(pos.real) + abs(pos.imag))

print(nav(heading['E'], part=1))
print(nav(10+1j, part=2))
