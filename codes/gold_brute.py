with open('Validation/annex3/txt/006_GoldCode2046hex210prns.txt') as f:
    lines = [line.strip().strip('\",') for line in f if line.strip() and not line.startswith('#') and '\"' in line]

hex1 = lines[0]
hex2 = lines[1]

def get_target(hex_str):
    t = []
    for c in hex_str:
        val = int(c, 16)
        t.extend([(val >> 3) & 1, (val >> 2) & 1, (val >> 1) & 1, val & 1])
    return t[2:2+2046]

target1 = get_target(hex1)
target2 = get_target(hex2)

def sim_right(m1, m2, delay, target):
    g1, g2 = 0x7FF, 0x7FF
    for _ in range(delay):
        masked = g2 & m2
        fb = 0
        for j in range(11): 
            fb ^= (masked >> j) & 1
        g2 = (g2 >> 1) | (fb << 10)
    for i in range(2046):
        out1 = g1 & 1
        masked1 = g1 & m1
        fb1 = 0
        for j in range(11): 
            fb1 ^= (masked1 >> j) & 1
        g1 = (g1 >> 1) | (fb1 << 10)
        
        out2 = g2 & 1
        masked2 = g2 & m2
        fb2 = 0
        for j in range(11): 
            fb2 ^= (masked2 >> j) & 1
        g2 = (g2 >> 1) | (fb2 << 10)
        
        if (out1 ^ out2) != target[i]:
            return False
    return True

found = False
# Test common masks for x^11 + x^2 + 1
# Taps could be (0, 9) -> 0x201, (1, 10) -> 0x402, (2, 10) -> 0x404
# Let's just test a few specific ones to be fast.
m1_candidates = [0x402, 0x201, 0x100, 0x005, 0x006, 0x204, 0x500]
# x^11 + x^8 + x^5 + x^2 + 1
# Taps could be (10, 7, 4, 1) -> 0x492, (0, 3, 6, 9) -> 0x249, (2, 5, 8, 10)
m2_candidates = [0x492, 0x249, 0x124, 0x125, 0x24A, 0x494, 0x524, 0x429]

print("Testing right shift with specific candidates...")
for m1 in m1_candidates:
    for m2 in m2_candidates:
        if sim_right(m1, m2, 1845, target1) and sim_right(m1, m2, 1071, target2):
            print(f'FOUND RIGHT SHIFT! m1={hex(m1)}, m2={hex(m2)}')
            found = True
            break
    if found: break

if not found:
    print("Testing left shift...")
    def sim_left(m1, m2, delay, target):
        g1, g2 = 0x7FF, 0x7FF
        for _ in range(delay):
            masked = g2 & m2
            fb = 0
            for j in range(11): 
                fb ^= (masked >> j) & 1
            g2 = ((g2 << 1) & 0x7FF) | fb
        for i in range(2046):
            out1 = (g1 >> 10) & 1
            masked1 = g1 & m1
            fb1 = 0
            for j in range(11): 
                fb1 ^= (masked1 >> j) & 1
            g1 = ((g1 << 1) & 0x7FF) | fb1
            
            out2 = (g2 >> 10) & 1
            masked2 = g2 & m2
            fb2 = 0
            for j in range(11): 
                fb2 ^= (masked2 >> j) & 1
            g2 = ((g2 << 1) & 0x7FF) | fb2
            
            if (out1 ^ out2) != target[i]:
                return False
        return True
    
    for m1 in m1_candidates:
        for m2 in m2_candidates:
            if sim_left(m1, m2, 1845, target1) and sim_left(m1, m2, 1071, target2):
                print(f'FOUND LEFT SHIFT! m1={hex(m1)}, m2={hex(m2)}')
                found = True
                break
        if found: break

print("Done testing candidates.")
