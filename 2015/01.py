with open('../aocinput/2015-01-input.txt') as f:
    data = f.readline().rstrip('\n')

# Part 1
print(data.count('(') - data.count(')'))

# Part 2
i = floor = 0
while floor >= 0:
    floor += 1 if data[i] == '(' else -1
    i += 1
print(i)
