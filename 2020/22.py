def read():
    with open('../aocinput/2020-22-example.txt') as f:
        return [list(map(int, d.strip().split('\n')[1:])) for d in f.read().split('\n\n')]

def deckid(deck):
    id = weight = 0
    for card in deck[::-1]:
        weight += 1
        id += weight * card
    return id

def gameid(decks):
    return [deckid(d) for d in decks]

# Part 1
def combat1(decks):
    while len(decks[0]) and len(decks[1]):
        draw = [d.pop(0) for d in decks]
        w = 0 if draw[0] > draw[1] else 1
        decks[w].extend([draw[w], draw[1 - w]])
    return sum(gameid(decks))

print(combat1(read()))

# Part 2
def combat2(decks):
    state = set()
    while len(decks[0]) and len(decks[1]):
        i, j = gameid(decks)
        id = 50000 * i + j  # unique enough (max = sum(squares(1..50)) = 42925)
        if id in state:
            return 0  # player 1 wins
        else:
            state.add(id)
        draw = [d.pop(0) for d in decks]
        if len(decks[0]) >= draw[0] and len(decks[1]) >= draw[1]:
            if max(decks[0][:draw[0]]) > max(decks[1][:draw[1]]):
                w = 0
            else:
                w = combat2([decks[0][:draw[0]], decks[1][:draw[1]]])
        else:
            w = 0 if draw[0] > draw[1] else 1
        decks[w].extend([draw[w], draw[1 - w]])
    return 0 if len(decks[0]) else 1

decks = read()
print(deckid(decks[combat2(decks)]))
