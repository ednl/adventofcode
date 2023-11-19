from collections import Counter

with open('../aocinput/2020-10-input.txt') as file:
	data = [int(line) for line in file]

data.append(0)
data.sort()
data.append(data[-1] + 3)

# Part 1
c = Counter([data[i + 1] - data[i] for i in range(len(data) - 1)])
print(c[1] * c[3])

# Part 2
plen = data[-1] + 1
paths = [0] * plen
paths[0] = 1
for i in range(1, plen):
	for j in range(i - 3, i):
		if j in data:
			paths[i] += paths[j]
print(paths[-1])
