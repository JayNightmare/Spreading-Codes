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

def next_state1(state):
    fb = 0
    for j in range(11): fb ^= ((state & m1) >> j) & 1
    return ((state << 1) & 0x7FF) | fb

def next_state2(state):
    fb = 0
    for j in range(11): fb ^= ((state & m2) >> j) & 1
    return ((state << 1) & 0x7FF) | fb

# Build matrix
# We want to find A and B such that target[i] = out1(A_i) ^ out2(B_i)
# out1(A) is bit 10 of A. out2(B) is bit 10 of B.
# Let basis for A be 1, 2, 4... 1024.
# Same for B.

basis1 = []
basis2 = []
for k in range(11):
    state = 1 << k
    seq = []
    for _ in range(22):
        seq.append((state >> 10) & 1)
        state = next_state1(state)
    basis1.append(seq)

for k in range(11):
    state = 1 << k
    seq = []
    for _ in range(22):
        seq.append((state >> 10) & 1)
        state = next_state2(state)
    basis2.append(seq)

# Matrix M is 22x22. columns are basis1 then basis2. rows are time 0..21
M = []
for i in range(22):
    row = []
    for k in range(11): row.append(basis1[k][i])
    for k in range(11): row.append(basis2[k][i])
    M.append(row)

# Gaussian elimination over GF(2)
# M * X = target[:22]
aug = [M[i] + [target[i]] for i in range(22)]

for i in range(22):
    # find pivot
    p = -1
    for j in range(i, 22):
        if aug[j][i] == 1:
            p = j
            break
    if p == -1:
        print('Singular!')
        exit(1)
    # swap
    aug[i], aug[p] = aug[p], aug[i]
    # eliminate
    for j in range(22):
        if i != j and aug[j][i] == 1:
            for k in range(23):
                aug[j][k] ^= aug[i][k]

A_val = 0
B_val = 0
for k in range(11):
    if aug[k][22]: A_val |= (1 << k)
    if aug[k+11][22]: B_val |= (1 << k)

print(f'Solved! Initial state 1: {hex(A_val)}, Initial state 2: {hex(B_val)}')

# Verify
g1 = A_val
g2 = B_val
seq = []
for _ in range(2046):
    out1 = (g1 >> 10) & 1
    g1 = next_state1(g1)
    out2 = (g2 >> 10) & 1
    g2 = next_state2(g2)
    seq.append(out1 ^ out2)

if seq == target:
    print('VERIFIED! Sequence matches perfectly!')
else:
    print('Failed verification.')
