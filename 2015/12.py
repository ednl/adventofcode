with open('../aocinput/2015-12-input.txt') as f:
    data = f.read().strip()

# Part 1
import re
num = re.compile(r'-?\d+')
print(sum(map(int, re.findall(num, data))))

# Part 2
import json
def sumjson(obj):
    if isinstance(obj, int):
        return obj
    n = 0
    if isinstance(obj, dict):
        for v in obj.values():
            if v == 'red':
                return 0
            else:
                n += sumjson(v)
    elif isinstance(obj, list):
        for v in obj:
            n += sumjson(v)
    return n
print(sumjson(json.loads(data)))
