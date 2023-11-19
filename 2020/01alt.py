with open('../aocinput/2020-01-input.txt') as f:
    data = sorted(map(int, list(f)))

count = len(data)
target = 2020

def findsum2():
	i = 0
	j = count - 1
	while i < j:
		s = data[i] + data[j]
		if s < target:
			i += 1
		elif s > target:
			j -= 1
		else:
			return data[i] * data[j]

def findsum3():
	for i in range(count - 2):
		j = i + 1
		k = count - 1
		while j < k:
			s = data[i] + data[j] + data[k]
			if s < target:
				j += 1
			elif s > target:
				k -= 1
			else:
				return data[i] * data[j] * data[k]

print(findsum2())
print(findsum3())
