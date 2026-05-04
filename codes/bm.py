with open('Validation/annex3/txt/006_GoldCode2046hex210prns.txt') as f:
    lines = [line.strip().strip('\",') for line in f if line.strip() and not line.startswith('#') and '\"' in line]

hex1 = lines[0]
target = []
for c in hex1:
    val = int(c, 16)
    target.extend([(val >> 3) & 1, (val >> 2) & 1, (val >> 1) & 1, val & 1])
target = target[2:2+2046]

def berlekamp_massey(s):
    n = len(s)
    c = [1] + [0] * (n - 1)
    b = [1] + [0] * (n - 1)
    l = 0
    m = -1
    for i in range(n):
        d = 0
        for j in range(l + 1):
            d ^= s[i - j] & c[j]
        if d == 1:
            t = c[:]
            for j in range(n - i + m):
                c[i - m + j] ^= b[j]
            if 2 * l <= i:
                l = i + 1 - l
                m = i
                b = t
    return l, c[:l + 1]

l, c = berlekamp_massey(target)
print('Linear complexity:', l)
print('Polynomial c(x):', [i for i, val in enumerate(c) if val])
