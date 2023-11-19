import numpy as np

puzzle = 33100000
street =  1000000
bag1  =        10
bag2  =        11
visit =        50

pres = np.ones(street, dtype=np.uint32) * bag1
for elf in range(2, street):
    pres[elf::elf] += elf * bag1
print("Part 1:", np.nonzero(pres >= puzzle)[0][0])

pres = np.ones(street, dtype=np.uint32) * bag2
for elf in range(2, street):
    pres[elf:(elf * visit) + 1:elf] += elf * bag2
print("Part 2:", np.nonzero(pres >= puzzle)[0][0])
