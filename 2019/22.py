deck  = 119315717514047
times = 101741582076661
pos   = 2020

shuffle = {
	'deal with increment ': lambda x,m,a,b: ( a * x % m,  b * x  % m),
	'deal into new stack' : lambda _,m,a,b: (-a     % m, (m-1-b) % m),
	'cut '                : lambda x,m,a,b: ( a        , (b-x)   % m)}

a,b = 1,0
with open('../aocinput/2019-22-input.txt') as f:
	for line in f.read().strip().split('\n'):
		for action, func in shuffle.items():
			if line.startswith(action):
				arg = int(line[len(action):]) if action[-1] == ' ' else 0
				a,b = func(arg, deck, a, b)
				break

r = (b * pow(1-a, deck-2, deck)) % deck
print(f"Card at #{pos}: {((pos - r) * pow(a, times * (deck-2), deck) + r) % deck}")
