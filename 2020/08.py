# Initialise VM, memory size
vm = []
memsize = 0

# Build the VM from input
with open('../aocinput/2020-08-input.txt') as f:
	for str in (line.strip() for line in f):
		vm.append(dict(op=str[:3], arg=int(str[4:]), addr=memsize, ord=0))
		memsize += 1

def run():
	# Initialise accumulator, instruction pointer, execution order
	acc = ip = ord = 0
	# Reset execution order state
	for i in vm:
		i['ord'] = 0
	# Run the VM until loop or out of bounds
	while ip >= 0 and ip < memsize and vm[ip]['ord'] == 0:
		ord += 1
		vm[ip]['ord'] = ord
		if vm[ip]['op'] == 'acc':
			acc += vm[ip]['arg']
			ip += 1
		elif vm[ip]['op'] == 'jmp':
			ip += vm[ip]['arg']
		else:
			ip += 1
	# Return instruction pointer, accumulator
	return ip, acc

# Part 1
ip, acc = run()
print(acc)

# Part 2
def flip(i):
	if i >= 0 and i < memsize:
		if vm[i]['op'] == 'jmp':
			vm[i]['op'] = 'nop'
		elif vm[i]['op'] == 'nop':
			vm[i]['op'] = 'jmp'

# Retain all jmp and nop instructions that were executed
debug = [i for i in vm if i['ord'] != 0 and (i['op'] == 'jmp' or i['op'] == 'nop')]

# Sort in reverse execution order (probably good for speed)
debug.sort(key=lambda i: i['ord'], reverse=True)

# Save the memory addresses
debug = [i['addr'] for i in debug]

# Flip instructions at those addresses one by one
# and re-run until no more loop
for mempointer in debug:
	flip(mempointer)
	ip, acc = run()
	if ip == memsize:
		break
	flip(mempointer)

print(acc)
