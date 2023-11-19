import re
rule = re.compile(r'^(\w+) can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds\.$')
finish = 2503
reindeer = []

with open('../aocinput/2015-14-input.txt') as f:
    for line in f:
        if m := rule.match(line.strip()):
            speed, fly, rest = map(int, m.group(2, 3, 4))
            reindeer.append({
                'dist': 0,
                'points': 0,
                'speed': speed,
                'fly': fly,
                'period': fly + rest})

lead = 0
for time in range(finish):
    for r in reindeer:
        if time % r['period'] < r['fly']:
            r['dist'] += r['speed']
        if r['dist'] > lead:
            lead = r['dist']
    for r in reindeer:
        if r['dist'] == lead:
            r['points'] += 1

maxdist = 0
maxpoints = 0
for r in reindeer:
    if r['dist'] > maxdist:
        maxdist = r['dist']
    if r['points'] > maxpoints:
        maxpoints = r['points']
print(maxdist, maxpoints)
