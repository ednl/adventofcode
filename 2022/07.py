dirsizes = []
inputfile = open("../aocinput/2022-07-input.txt")

def subdir():
    sum = 0
    for line in inputfile:
        a = line.split()
        try:
            sum += int(a[0])
        except:
            if a[1] == "cd":
                if a[2] == "..":
                    dirsizes.append(sum)
                    return sum
                else:
                    sum += subdir()
    dirsizes.append(sum)
    return sum

# Also fills dirsizes list
minsize = subdir() + 30_000_000 - 70_000_000

print(sum(x for x in dirsizes if x <= 100_000))  # example:    95437, input: 1845346
print(min(x for x in dirsizes if x >= minsize))  # example: 24933642, input: 3636703
