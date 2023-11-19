possible = {}
ingcount = {}
with open('../aocinput/2020-21-input.txt') as f:
    for line in f:
        i, a = line.split(' (contains ')
        ingredients = set(i.split())
        allergens = a[:-2].split(', ')

        for i in ingredients:
            ingcount[i] = ingcount.get(i, 0) + 1

        for a in allergens:
            if a in possible:
                possible[a] &= ingredients
            else:
                possible[a] = ingredients.copy()

danger = {}
while len(possible):
    allergen = [a for a, i in possible.items() if len(i) == 1][0]
    ingredient = possible.pop(allergen)
    for a, i in possible.items():
        i -= i & ingredient
    danger[allergen] = ingredient.pop()

# Part 1
bad = danger.values()
print(sum([c for i, c in ingcount.items() if i not in bad]))

# Part 2
sortbyallergen = sorted([(a, i) for a, i in danger.items()])
print(','.join([i for _, i in sortbyallergen]))
