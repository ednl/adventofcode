masks = []
memwrites = []
tmp = []

with open('../aocinput/2020-14-input.txt') as f:
	for line in f:
		if line[1] == 'a':
			masks.append(line[7:-1])
			if len(tmp):
				memwrites.append(tmp)
				tmp = []
		else:
			tmp.append(list(map(int, line[4:].split('] = '))))

memwrites.append(tmp)
data = list(zip(masks, memwrites))

# Part 1
mem = {}
for mask, store in data:
	set_and = int(mask.replace('X', '1'), 2)
	unset_or = int(mask.replace('X', '0'), 2)
	for addr, val in store:
		mem[addr] = val & set_and | unset_or

print(sum(mem.values()))

# Part 2
mem = {}
for mask, memwrites in data:
	unset_or = int(mask.replace('X', '0'), 2)
	posx, _ = zip(*filter(lambda x: x[1] == 'X', zip(range(len(mask) - 1, -1, -1), mask)))
	for xvalue in range(1 << len(posx)):
		ma = -1
		mo = unset_or
		bitpos = 1 << len(posx)
		for ix in posx:           # adjust the bits at every position ix where mask=X
			bitpos >>= 1          # check every bit in the xvalue
			if xvalue & bitpos:
				mo |= (1 << ix)   # set bit in the or-mask at position ix
			else:
				ma &= ~(1 << ix)  # unset bit in the and-mask at position ix
		for addr, val in memwrites:
			mem[addr & ma | mo] = val

print(sum(mem.values()))
