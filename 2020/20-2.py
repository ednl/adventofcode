import numpy as np
from numpy.lib.function_base import place

ids = []
tiles = np.zeros((1,10,10), dtype=np.uint8)  #stub
with open('../aocinput/2020-20-input.txt') as f:
    tiledata = f.read().strip().split('\n\n')
    for td in tiledata:
        t = td.strip().split('\n')
        ids.append(int(t[0][5:-1]))
        tile = (np.array([[list(s) for s in t[1:]]]) == '#').astype(np.uint8)
        tiles = np.concatenate((tiles, tile))
tiles = tiles[1:,:,:]  # delete stub

# Original: t,r,b,l (=top,right,bottom,left)
# Rotations: r,-b,l,-t | -b,-l,-t,-r | -l,t,-r,b
# Flipped horizontally: -t,l,-b,r
# Rotations: l,b,r,t | b,-r,t,-l | -r,-t,-l,-b

# tiles = np.rot90(tiles, axes=(1, 2))
# tiles = np.rot90(tiles, axes=(1, 2))
# place = np.pad(place, ((1,1),), mode='constant')

# place = np.zeros((1,1))
# place[0,0] = ids[0]
# img = 

monster = (np.array([list(s) for s in [
    '                  # ',
    '#    ##    ##    ###',
    ' #  #  #  #  #  #   '
]]) == '#').astype(np.uint8)
print(monster)
print(monster.shape)
