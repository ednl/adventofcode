# Code by Eric Wastl / topaz / fizbin via
# https://topaz.github.io/paste/#XQAAAQBGBwAAAAAAAAA0m0pnuFI8c+qagMoNTEcTIfyUW1xgFU6d32tV7mCeW4wKdnKg9I28Rka6MBN0RahzCZagHwSfmD0dUh8gxeYdKFygU0rIloMzPa/MvAsy16GMe76wPUM+t8jx9eKdZysHaKv8MYfjY12CAikYbImCM9+I1m4r5N08UwK+yxN8L3WDRjiJGvYyShj8W1LGSA4Ipz6ujoU8Gdp2wEtzseHe/hjGJjw1pBz3NQV7Y1Os+WriH4HWCo4VWeru+1sQjA5Zguhs7XOKPUg7rKRI0VYdAmazkwM34gu59EtSaM96kBlNMe427hieavqaYYSGCTO/+uRd7NYMi3f9Ky0AYD43aDJITstFzXtPA54dtsHEok1iNGQNSDKj30MMN1njTf7AEXnLa1D1Pe9fiMokFULv2E5PswEGzfPR/k3v7Ke/JRSCnv4zX5jox3lJdbY6BiAZ0uZ5MnCL9JA+AiunaimgvRK3NnVg3byhyh5o4hWxnIvDESJRsT5WembAIYJFWPtjsXQXr8hpC6K6RBZUDDV2wDvA5y8AwVUPDDBy8o3hfJ/p9AjcmBbxHfplOVECiJhaC65duJUx7JPLhvwbrQaguz8jOxsBb+0ye1ZbiQXwiSsmgOmpKsfEbidSRA62h/eft2ohERRcBE5tPRYiuiaUT7/Nm7xgs/qsz3MfkHi6JI4SsKqH3uOnFnUSlGC4xdneWVC3l4QqmyidDyzPuv5i2l9Gaxi454XFgNoiVEC3IBsAJFQ5z+7W4I9vIS5E0Ig8Ir6mV+6D+40Lz2wNPDG8lhOo3rmcZ9kE9KZBWXS8YSkR8H262UymGjeFOp77ZJK07fTN2voFdNeB5z5BtoK3hP/tvu7W

import numpy as np
import sys

start_a = 783
start_b = 325

if len(sys.argv) >= 3:
    start_a = int(sys.argv[1])
    start_b = int(sys.argv[2])

a_mul0 = 16807
b_mul0 = 48271
base = 2147483647

a_arr0 = np.array([(a_mul0 * start_a) % base], dtype=np.uint64)
b_arr0 = np.array([(b_mul0 * start_b) % base], dtype=np.uint64)

def double_arrays(a_arr, a_mul, b_arr, b_mul):
    new_a_arr = a_arr.copy()
    new_b_arr = b_arr.copy()
    new_a_arr *= a_mul
    new_a_arr %= base
    new_b_arr *= b_mul
    new_b_arr %= base
    return (np.concatenate((a_arr, new_a_arr)), (a_mul * a_mul) % base,
            np.concatenate((b_arr, new_b_arr)), (b_mul * b_mul) % base)

work_tuple = (a_arr0, a_mul0, b_arr0, b_mul0)

forty_million = 40 * 1000 * 1000
five_million = 5 * 1000 * 1000
arrlim = forty_million // 50
while len(work_tuple[0]) < arrlim:
    work_tuple = double_arrays(*work_tuple)

work_a_arr = work_tuple[0][0:arrlim].copy()
work_a_mul = pow(a_mul0, arrlim, base)
work_b_arr = work_tuple[2][0:arrlim].copy()
work_b_mul = pow(b_mul0, arrlim, base)
count1 = 0
workrounds = 0
new_a_out = np.array([], dtype=np.uint64)
new_b_out = np.array([], dtype=np.uint64)
while (workrounds < 50) or (len(new_a_out) < five_million) or (len(new_b_out) < five_million):
    if workrounds < 50:
        count1 += ((work_a_arr & 0xFFFF) == (work_b_arr & 0xFFFF)).sum()
    # collect new vals
    if len(new_a_out) < five_million:
        new_a_out = np.concatenate((new_a_out, work_a_arr[work_a_arr % 4 == 0] & 0xFFFF))
    if len(new_b_out) < five_million:
        new_b_out = np.concatenate((new_b_out, work_b_arr[work_b_arr % 8 == 0] & 0xFFFF))
    work_a_arr *= work_a_mul
    work_a_arr %= base
    work_b_arr *= work_b_mul
    work_b_arr %= base
    workrounds += 1
    # print(workrounds)

print(count1)
print(np.sum(new_a_out[0:five_million] == new_b_out[0:five_million]))
