mem1 = {}
mem2 = {}
mask_or = 0
mask_and = -1
posx = []

with open('../aocinput/2020-14-input.txt') as f:
    for line in f:
        if line[1] == 'a':
            mask = line[7:-1]
            mask_or = int(mask.replace('X', '0'), 2)   # or-mask (set bits where mask=1)

            # Part 1
            mask_and = int(mask.replace('X', '1'), 2)  # and-mask (unset bits where mask=0)

            # Part 2
            # List of bit positions of all X's in the mask (MSB first)
            posx, _ = zip(*filter(lambda x: x[1] == 'X', zip(range(len(mask) - 1, -1, -1), mask)))

        else:
            addr, val = map(int, line[4:].split('] = '))

            # Part 1
            mem1[addr] = val & mask_and | mask_or

            # Part 2
            addr |= mask_or  # first adjustment according to the problem (set bits where mask=1)
            # Count through all possible values for the bits that were X
            for xvalue in range(1 << len(posx)):
                ma = -1                   # and-mask (unset bits where mask=0, initially all 1)
                mo = 0                    # or-mask (set bits where mask=1, initially all 0)
                bitpos = 1 << len(posx)
                for ix in posx:           # adjust the bits at every position ix where mask=X
                    bitpos >>= 1          # check every bit in the xvalue
                    if xvalue & bitpos:
                        mo |= (1 << ix)   # set bit in the or-mask at position ix
                    else:
                        ma &= ~(1 << ix)  # unset bit in the and-mask at position ix
                mem2[addr & ma | mo] = val

print(sum(mem1.values()))
print(sum(mem2.values()))
