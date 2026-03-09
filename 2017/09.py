a = list(open("../aocinput/2017-09-input.txt").read())
index: int = 0
group: int = 0
score: int = 0
count: int = 0
garbage: bool = False
while index < len(a):
    if garbage:
        if a[index] == '!':
            index += 1
        elif a[index] == '>':
            garbage = False
        else:
            count += 1
    else:
        if a[index] == '{':
            group += 1
        elif a[index] == '}':
            score += group
            group -= 1
        elif a[index] == '<':
            garbage = True
    index += 1
print(score, count)  # 20530 9978
