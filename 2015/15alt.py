import numpy as np
import re
num = re.compile(r'-?\d+')
with open('../aocinput/2015-15-input.txt') as f:
    properties = np.array(list(map(int, re.findall(num, f.read()))), dtype=np.int32).reshape((4, 5)).T

ingredients = properties[:4]

def cookie(ingredients, teaspoons):
    return np.prod(np.dot(ingredients, teaspoons))

def hiscore(ingredients, teaspoons, grad):
    spoon = np.array(teaspoons, dtype=np.int32)
    score = cookie(ingredients, spoon)
    maxscore = score
    maxspoon = spoon
    while True:
        for ds in grad:
            tryspoon = spoon + ds
            tryscore = cookie(ingredients, tryspoon)
            if tryscore > maxscore:
                maxscore = tryscore
                maxspoon = tryspoon
        if maxscore == score:
            return score
        score = maxscore
        spoon = maxspoon

grad = np.insert(np.identity(3, dtype=np.int32), 0, -1, axis=1)
for i in range(1, 4):
    grad = np.append(grad, np.insert(np.identity(3, dtype=np.int32), i, -1, axis=1), axis=0)
print("Part 1:", hiscore(ingredients, [25,25,25,25], grad))

# Part 2
# New calories restriction  : 3a+3b+8c+8d=500
# Same teaspoons restriction:  a+ b+ c+ d=100
# => a+b=60, c+d=40

grad = np.array([
    [-1, 1, 0, 0],
    [ 1,-1, 0, 0],
    [ 0, 0,-1, 1],
    [ 0, 0, 1,-1]
], dtype=np.int32)
print("Part 2:", hiscore(ingredients, [30,30,20,20], grad))