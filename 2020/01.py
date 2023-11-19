with open('../aocinput/2020-01-input.txt') as f:
    a = sorted(map(int, list(f)))

print('Good')
for i in range(len(a) - 2):
	if a[i] >= 2020:
		break
	else:
		for j in range(i + 1, len(a) - 1):
			sum2 = a[i] + a[j]
			if sum2 > 2020:
				break
			elif sum2 == 2020:
				print('2:', a[i] * a[j])
				break
			else:
				for k in range(j + 1, len(a)):
					sum3 = sum2 + a[k]
					if sum3 > 2020:
						break
					elif sum3 == 2020:
						print('3:', a[i] * a[j] * a[k])
						break

print('\nBad, but correct')
for i in a:
	for j in a:
		if (i + j == 2020):
			print('2:', i * j)
		for k in a:
			if (i + j + k == 2020):
				print('3:', i * j * k)

print('\nBaaaaaaaad, but correct')
from random import sample
from functools import reduce

for i in range(2, 4):
	sub = []
	n = 0
	while n != 2020:
		sub = sample(a, i)
		n = sum(sub)
	print(i, reduce((lambda x, y: x * y), sub))
