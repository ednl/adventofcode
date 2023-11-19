with open('../aocinput/2020-18-input.txt') as f:
    data = [line.strip() for line in f]

# Operators with function for 2 operands, and precedence level
op = {
    '+': {'f': lambda x, y: x + y, 'p': 1},
    '*': {'f': lambda x, y: x * y, 'p': 1}
}

# Shunting-yard algorithm, all left-associative
# https://en.wikipedia.org/wiki/Shunting-yard_algorithm
def shunt(infix):
    res = []
    stack = []
    for token in infix:
        if token in '0123456789':
            res.append(int(token))
        elif token in op:
            while len(stack):
                x = stack.pop()
                if x == '(' or op[x]['p'] < op[token]['p']:
                    stack.append(x)
                    break
                res.append(op[x]['f'](res.pop(), res.pop()))
            stack.append(token)
        elif token == '(':
            stack.append(token)
        elif token == ')':
            while len(stack):
                x = stack.pop()
                if x == '(':
                    break
                res.append(op[x]['f'](res.pop(), res.pop()))
    while len(stack):
        x = stack.pop()
        res.append(op[x]['f'](res.pop(), res.pop()))
    return res[0]

# Part 1
print('1:', sum(map(shunt, data)))

# Part 2
op['+']['p'] = 2
print('2:', sum(map(shunt, data)))
