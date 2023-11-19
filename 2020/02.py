#!/usr/bin/env python3

with open('../aocinput/2020-02-input.txt', 'rt') as f:
	txt = list(f)

bad = 0
good = 0

for line in txt:
	fld = line.split()
	num = list(map(int, fld[0].split('-')))
	ch = fld[1].strip(':')
	pw = fld[2]

	# Part 1
	# char must occur from num0 to num1 times in string
	n = pw.count(ch)
	if n < num[0] or n > num[1]:
		bad += 1

	# Part 2
	# 1 of 2 chars at indices num0 and num1 (1-based) must be char
	if (pw[num[0] - 1] == ch) != (pw[num[1] - 1] == ch):
		good += 1

print('bad:', bad)
print('good:', good)
