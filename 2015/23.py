# Advent of Code 2015 Day 23: Opening the Turing Lock
# Input is an assembly program that simulates the Collatz Conjecture: for two different
#   initial values it counts the number of steps until the value is back to one.
# Part 1: (((((((0+1)*3*3*3+1)*3+1)*3+1+1)*3+1+1)*3+1+1)*3+1+1)*3 = 20895 => 255
# Part 2: (((((1*3*3*3*3+1+1)*3+1)*3+1+1)*3+1+1)*3+1)*3*3         = 60975 => 334

import re
cmd = re.compile(r'^(\w+) (\w)?(?:, )?((?:\+|-)\d+)?$')

with open('../aocinput/2015-23-input.txt') as f:
    # Subtract 1 from jump (offset) to enable ip++ for every instruction
    mem = [(i, r, j if j is None else int(j) - 1) for s in f for i, r, j in [cmd.match(s.strip()).groups()]]

# Run assembly program
def run(a: int) -> int:
    reg = {'a': a, 'b': 0}
    ip = 0
    while ip >= 0 and ip < len(mem):
        i, r, j = mem[ip]
        if i == 'inc':
            reg[r] += 1
        elif i == 'hlf':
            reg[r] //= 2
        elif i == 'tpl':
            reg[r] *= 3
        elif i == 'jmp' or (i == 'jie' and reg[r] % 2 == 0) or (i == 'jio' and reg[r] == 1):
            ip += j
        ip += 1
    return reg['b']

print(run(0), run(1))

# Validation
def collatz(a: int) -> int:
    b = 0
    while (a > 1):
        b += 1
        if a % 2:
            a = a * 3 + 1
        else:
            a //= 2
    return b

part1 = (((((((0+1)*3*3*3+1)*3+1)*3+1+1)*3+1+1)*3+1+1)*3+1+1)*3
part2 = (((((1*3*3*3*3+1+1)*3+1)*3+1+1)*3+1+1)*3+1)*3*3
print(collatz(part1), collatz(part2))
