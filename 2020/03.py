data = list(open('../aocinput/2020-03-input.txt'))
width = len(data[0].strip())

slope = [[1,1], [3,1], [5,1], [7,1], [1,2]]
product = 1

for dx, dy in slope:
	x = y = trees = 0
	while y < len(data):
		trees += 1 if data[y][x % width] == '#' else 0
		x, y = x + dx, y + dy
	product *= trees
	print(dx, dy, trees)

print(product)
