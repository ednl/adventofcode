# Conway's constant
# https://www.youtube.com/watch?v=ea7lJkEhytA
# https://mathworld.wolfram.com/ConwaysConstant.html
# https://www.wolframalpha.com/input/?i=conway’s+constant
# https://en.wikipedia.org/wiki/Look-and-say_sequence

from array import array
from math import log, ceil

say = '1113222113'
cc = 1.30357727

def looksay(a, b, len):
    i = k = 0
    while i < len:
        j = i + 1
        while j < len and a[j] == a[i]:
            j += 1
        b[k] = j - i
        b[k + 1] = a[i]
        k += 2
        i = j
    return k

def lookandsay(n):
    length = len(say)
    maxlen = ceil(pow(cc, n + ceil(log(length, cc))))
    a = array('B', [0] * maxlen)
    b = array('B', [0] * maxlen)
    a[:length] = array('B', map(int, list(say)))
    for i in range(2, n + 1, 2):
        length = looksay(a, b, length)
        length = looksay(b, a, length)
        print(i, length)

lookandsay(50)
