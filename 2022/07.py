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

dirsizes = []
inputfile = open("../aocinput/2022-07-input.txt")
minsize = subdir() + 30_000_000 - 70_000_000

print(sum(x for x in dirsizes if x <= 100_000))  # 1845346
print(min(x for x in dirsizes if x >= minsize))  # 3636703
