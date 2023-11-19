from collections import deque
from itertools import combinations

with open('../aocinput/2020-09-input.txt') as f:
	a = [int(line) for line in f]

qlen = 25
mynum = 0

# Part 1
q = deque(a[:qlen], maxlen=qlen)
for n in a[qlen:]:
	s = 0
	for duo in combinations(q, 2):
		s = sum(duo)
		if s == n:
			break
	if s != n:
		mynum = n
		break
	q.append(n)  # deque with maxlen auto-pops

print(mynum)

# Part 2
def weakness():
	for i in range(len(a) - 1):
		s = a[i]
		for j in range(i + 1, len(a)):
			s += a[j]
			if s > mynum:
				break
			if s == mynum:
				return min(a[i:j + 1]) + max(a[i:j + 1])

print(weakness())
