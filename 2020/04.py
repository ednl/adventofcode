import re

validregex = {
	'byr': r'^(19[2-9]\d|200[0-2])$',
	'iyr': r'^20(1\d|20)$',
	'eyr': r'^20(2\d|30)$',
	'hgt': r'^(1([5-8]\d|9[0-3])cm|(59|6\d|7[0-6])in)$',
	'hcl': r'^#[0-9a-f]{6}$',
	'ecl': r'^(amb|blu|brn|gry|grn|hzl|oth)$',
	'pid': r'^\d{9}$'
}

compiledregex = {}
for k, v in validregex.items():
	compiledregex[k] = re.compile(v)

def isvalid(pp):
	if len(pp) < len(validregex):
		return 0
	for k, regex in compiledregex.items():
		if not (k in pp):
			return 0
		if not regex.match(pp[k]):
			return 0
	return 1

data = list(open('../aocinput/2020-04-input.txt'))
passport = {}
valid = 0

for line in data:
	if line == '\n':
		valid += isvalid(passport)
		passport = {}
		continue
	fields = line.split()
	for field in fields:
		k, v = field.split(':')
		passport[k] = v

valid += isvalid(passport)
print(valid)
