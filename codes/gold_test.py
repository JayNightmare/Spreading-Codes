with open('Validation/annex3/txt/006_GoldCode2046hex210prns.txt') as f:
    lines = [line.strip().strip('\",') for line in f if line.strip() and not line.startswith('#') and '\"' in line]

hex1 = lines[0]
target = []
for c in hex1:
    val = int(c, 16)
    target.extend([(val >> 3) & 1, (val >> 2) & 1, (val >> 1) & 1, val & 1])
target = target[2:2+2046]

def gen_gold(m1, m2, delay):
    g1, g2 = 0x7FF, 0x7FF
    seq = []
    for _ in range(delay):
        fb = 0
        for j in range(11): fb ^= ((g2 & m2) >> j) & 1
        g2 = ((g2 << 1) & 0x7FF) | fb
    for _ in range(2046 * 2):
        out1 = (g1 >> 10) & 1
        fb1 = 0
        for j in range(11): fb1 ^= ((g1 & m1) >> j) & 1
        g1 = ((g1 << 1) & 0x7FF) | fb1
        
        out2 = (g2 >> 10) & 1
        fb2 = 0
        for j in range(11): fb2 ^= ((g2 & m2) >> j) & 1
        g2 = ((g2 << 1) & 0x7FF) | fb2
        seq.append(out1 ^ out2)
    return seq

seq = gen_gold(0x005, 0x125, 1845)
for offset in range(2046):
    if seq[offset:offset+2046] == target:
        print('MATCH at offset:', offset)
        exit(0)

seq2 = gen_gold(0x402, 0x492, 1845)
for offset in range(2046):
    if seq2[offset:offset+2046] == target:
        print('MATCH at offset:', offset)
        exit(0)

print('No offset matched.')
