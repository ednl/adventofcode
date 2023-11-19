with open('../aocinput/2015-05-input.txt') as f:
    data = [line.strip() for line in f]

vowels = list('aeiou')
naughty = ['ab', 'cd', 'pq', 'xy']

# Part 1
nice = 0
for string in data:
    vowelcount = 0
    double = False
    proper = True
    prev = ''
    for c in string:
        if vowelcount < 3:
            if c in vowels:
                vowelcount += 1
        if not double:
            if c == prev:
                double = True
        if proper:
            if prev + c in naughty:
                proper = False
        prev = c
    if vowelcount >= 3 and double and proper:
        nice += 1
print(nice)

# Part 2
def nonoverlapdouble(s):
    for i in range(len(s) - 1):
        p = s[i:i + 2]
        for j in range(i + 2, len(s) - 1):
            if s[j:j + 2] == p:
                return True
    return False

def repeatonebetween(s):
    for i in range(len(s) - 2):
        if s[i] == s[i + 2]:
            return True
    return False

nice = 0
for string in data:
    if repeatonebetween(string) and nonoverlapdouble(string):
        nice += 1
print(nice)
