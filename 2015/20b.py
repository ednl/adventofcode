from array import array

puzzle = 33100000
street =  1000000
bag1  =        10
bag2  =        11
visit =        50

pres = array('I', [bag1] * street)
for elf in range(2, street):
    n = elf * bag1
    for house in range(elf, street, elf):
        pres[house] += n
for house in range(street):
    if pres[house] >= puzzle:
        print(house)
        break

pres = array('I', [bag2] * street)
for elf in range(2, street):
    n = elf * bag2
    m = min(elf * visit + 1, street)
    for house in range(elf, m, elf):
        pres[house] += n
for house in range(street):
    if pres[house] >= puzzle:
        print(house)
        break
