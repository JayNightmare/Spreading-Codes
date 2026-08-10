with open('Validation/annex3/txt/006_GoldCode2046hex210prns.txt') as f:
    lines = [line.strip().strip('\",') for line in f if line.strip() and not line.startswith('#') and '\"' in line]

hex1 = lines[0]
target = []
for c in hex1:
    val = int(c, 16)
    target.extend([(val >> 3) & 1, (val >> 2) & 1, (val >> 1) & 1, val & 1])
target = target[2:2+2046]

m1 = 0x500
m2 = 0x524

g1 = 0x7FF
seq1 = []
for _ in range(2047 * 3):
    out1 = (g1 >> 10) & 1
    fb1 = 0
    for j in range(11): fb1 ^= ((g1 & m1) >> j) & 1
    g1 = ((g1 << 1) & 0x7FF) | fb1
    seq1.append(out1)

g2 = 0x7FF
seq2 = []
for _ in range(2047 * 3):
    out2 = (g2 >> 10) & 1
    fb2 = 0
    for j in range(11): fb2 ^= ((g2 & m2) >> j) & 1
    g2 = ((g2 << 1) & 0x7FF) | fb2
    seq2.append(out2)

found = False
for d1 in range(2047):
    diff = [(target[i] ^ seq1[i + d1]) for i in range(2046)]
    s_diff = ''.join(map(str, diff))
    idx = ''.join(map(str, seq2)).find(s_diff)
    if idx != -1:
        print(f'FOUND! d1={d1}, d2={idx}')
        found = True
        break
if not found:
    print('No match found in python!')
