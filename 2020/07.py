tree = {}
mybag = 'shiny gold'
hasmybag = set()

with open('../aocinput/2020-07-input.txt') as f:
	for str in (line.rstrip('\n.') for line in f):
		root, leaves = str.split(' bags contain ')
		branch = {}
		for leaf in leaves.split(', '):
			words = leaf.split()
			name = ' '.join(words[1:-1])
			if name != 'other':
				branch[name] = int(words[0])
		tree[root] = branch

# Part 1
def check(outerbag):
	if outerbag in hasmybag:
		return True
	if mybag in tree[outerbag]:
		hasmybag.add(outerbag)
		return True
	for innerbag in tree[outerbag].keys():
		if check(innerbag):
			hasmybag.add(innerbag)
			return True
	return False

for bag in tree.keys():
	if not bag in hasmybag:
		if check(bag):
			hasmybag.add(bag)
print(len(hasmybag))

# Part 2
def whatsinthe(outerbag):
	bags = 0
	for innerbag, size in tree[outerbag].items():
		bags += size * (1 + whatsinthe(innerbag))
	return bags

print(whatsinthe(mybag))
