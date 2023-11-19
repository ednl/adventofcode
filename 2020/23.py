import array

def crabcups(labels, moves=100, cupcount=9):
    next = array.array('I', range(1, cupcount + 2))
    cups = array.array('I', map(int, labels))

    next[0] = next[-1] = cups[0]
    for i in range(len(cups) - 1):
        next[cups[i]] = cups[i + 1]
    next[cups[len(cups) - 1]] = cups[0] if cupcount == len(cups) else max(cups) + 1

    cur = 0
    for _ in range(moves):
        cur = next[cur]
        ins = cur - 1 if cur != 1 else cupcount
        p1 = next[cur]
        p2 = next[p1]
        p3 = next[p2]
        while ins == p1 or ins == p2 or ins == p3:
            ins -= 1
        if ins < 1:
            ins += cupcount
        next[p3], next[ins], next[cur] = next[ins], next[cur], next[p3]

    if cupcount < 10:
        i = next[1]
        while i != 1:
            print(i, end='')
            i = next[i]
        print()
    else:
        print(next[1] * next[next[1]])

# Part 1
data = '362981754'
crabcups(data)

# Part 2
crabcups(data, moves=10_000_000, cupcount=1_000_000)
