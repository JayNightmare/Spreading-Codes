#include <iostream>
#include <vector>

std::vector<int> generate_left(int delay) {
    uint16_t g1 = 0x7FF, g2 = 0x7FF;
    for(int i=0; i<delay; ++i) {
        uint8_t out = (g2 >> 10) & 1;
        uint8_t fb = ((g2 >> 10) & 1) ^ ((g2 >> 7) & 1) ^ ((g2 >> 4) & 1) ^ ((g2 >> 1) & 1);
        g2 = ((g2 << 1) & 0x7FF) | fb;
    }
    std::vector<int> res;
    for(int i=0; i<10; ++i) {
        uint8_t out1 = (g1 >> 10) & 1;
        uint8_t fb1 = ((g1 >> 10) & 1) ^ ((g1 >> 1) & 1);
        g1 = ((g1 << 1) & 0x7FF) | fb1;
        
        uint8_t out2 = (g2 >> 10) & 1;
        uint8_t fb2 = ((g2 >> 10) & 1) ^ ((g2 >> 7) & 1) ^ ((g2 >> 4) & 1) ^ ((g2 >> 1) & 1);
        g2 = ((g2 << 1) & 0x7FF) | fb2;
        
        res.push_back(out1 ^ out2);
    }
    return res;
}

std::vector<int> generate_right(int delay) {
    uint16_t g1 = 0x7FF, g2 = 0x7FF;
    for(int i=0; i<delay; ++i) {
        uint8_t out = g2 & 1;
        uint8_t fb = (g2 & 1) ^ ((g2 >> 3) & 1) ^ ((g2 >> 6) & 1) ^ ((g2 >> 8) & 1);
        g2 = (g2 >> 1) | (fb << 10);
    }
    std::vector<int> res;
    for(int i=0; i<10; ++i) {
        uint8_t out1 = g1 & 1;
        uint8_t fb1 = (g1 & 1) ^ ((g1 >> 8) & 1);
        g1 = (g1 >> 1) | (fb1 << 10);
        
        uint8_t out2 = g2 & 1;
        uint8_t fb2 = (g2 & 1) ^ ((g2 >> 3) & 1) ^ ((g2 >> 6) & 1) ^ ((g2 >> 8) & 1);
        g2 = (g2 >> 1) | (fb2 << 10);
        
        res.push_back(out1 ^ out2);
    }
    return res;
}

int main() {
    auto left = generate_left(1845);
    std::cout << "Left:  "; for(int x : left) std::cout << x; std::cout << "\n";
    auto right = generate_right(1845);
    std::cout << "Right: "; for(int x : right) std::cout << x; std::cout << "\n";
    std::cout << "Exp:   0001011101\n";
    return 0;
}
