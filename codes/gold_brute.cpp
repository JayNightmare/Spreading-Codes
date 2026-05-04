#include <iostream>
#include <vector>
#include <string>

uint8_t HexToNibble(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return 0;
}

std::vector<uint8_t> get_target(const std::string& hex_str) {
    std::vector<uint8_t> t;
    for (char c : hex_str) {
        int val = HexToNibble(c);
        t.push_back((val >> 3) & 1);
        t.push_back((val >> 2) & 1);
        t.push_back((val >> 1) & 1);
        t.push_back(val & 1);
    }
    std::vector<uint8_t> out;
    for(int i=2; i<2048; ++i) out.push_back(t[i]);
    return out;
}

int main() {
    std::string hex1 = "17590C193EB85C40632F5B9919BEEB8D010915A0971731A0037BB7ED5C5BDB551C10585B8CEE0A2F01957B1B026772D277822530FC184864FC55EA96C4FA07B9D80E8899B952F39030F91A3F1A8554FFD77331CC1D294177CB1A71C9A97A1B31B8D7C6CE1120C8E2788643099FA4E77213192F89C49782473E9710C0D4301055CBD30CF8503256C02674C88449A9948E229FDAA0176F733C86691CF773F21B8D5B4799505739CA59338756CDF4C57F5FF2127567C1BA7233AA5E70322E912F4DFB028870D5E5213F16BC459B93CC852F99866A0454ED7FFCB197CA3D90068257E4AF14BEB85A1C31F9E26AE546D3134D8C3C6543AB8C0712FC73C28714CF00CE";
    std::string hex2 = "130BFFDA4199FCFE84D51D04CC16A7A216A95236D07807721C02AF2371A22F2156062168D5767F97F4961B14957CA048C7F9566381809382DF67B89C76380FB2615E47E8FD0AE5BE2CCB9AD3A0E2BB729060BFC09685DA627C78697B3C5D7B90711A2566A47E626B9417B3539EAF75B569E427509BCFE8D1FC3145AFB5CD64344A1FB3ED490E984381568DFC5AF57E0D2BE349E93636BC6655848695EE89ED1745623C3218A43466CA240DECFA254E94868A851657046CB851EA0E9DD58B0F34312AAD2CC617956A2EF2D3F76B4FDB7C61D7D25F095B4159AF6089577A7CFA965E4D325042371651DDE2C4BB90E22148A9E5BFD2A2A7BB30C5B30F4F2BCFD228";
    auto t1 = get_target(hex1);
    auto t2 = get_target(hex2);

    auto sim_left = [](int m1, int m2, int delay, const std::vector<uint8_t>& target) {
        int g1 = 0x7FF, g2 = 0x7FF;
        for(int i=0; i<delay; ++i) {
            int masked = g2 & m2;
            int fb = 0;
            for(int j=0; j<11; ++j) fb ^= (masked >> j) & 1;
            g2 = ((g2 << 1) & 0x7FF) | fb;
        }
        for(int i=0; i<2046; ++i) {
            int out1 = (g1 >> 10) & 1;
            int masked1 = g1 & m1;
            int fb1 = 0;
            for(int j=0; j<11; ++j) fb1 ^= (masked1 >> j) & 1;
            g1 = ((g1 << 1) & 0x7FF) | fb1;
            
            int out2 = (g2 >> 10) & 1;
            int masked2 = g2 & m2;
            int fb2 = 0;
            for(int j=0; j<11; ++j) fb2 ^= (masked2 >> j) & 1;
            g2 = ((g2 << 1) & 0x7FF) | fb2;
            
            if ((out1 ^ out2) != target[i]) return false;
        }
        return true;
    };

    auto sim_right = [](int m1, int m2, int delay, const std::vector<uint8_t>& target) {
        int g1 = 0x7FF, g2 = 0x7FF;
        for(int i=0; i<delay; ++i) {
            int masked = g2 & m2;
            int fb = 0;
            for(int j=0; j<11; ++j) fb ^= (masked >> j) & 1;
            g2 = (g2 >> 1) | (fb << 10);
        }
        for(int i=0; i<2046; ++i) {
            int out1 = g1 & 1;
            int masked1 = g1 & m1;
            int fb1 = 0;
            for(int j=0; j<11; ++j) fb1 ^= (masked1 >> j) & 1;
            g1 = (g1 >> 1) | (fb1 << 10);
            
            int out2 = g2 & 1;
            int masked2 = g2 & m2;
            int fb2 = 0;
            for(int j=0; j<11; ++j) fb2 ^= (masked2 >> j) & 1;
            g2 = (g2 >> 1) | (fb2 << 10);
            
            if ((out1 ^ out2) != target[i]) return false;
        }
        return true;
    };

    for(int m1=0; m1<2048; ++m1) {
        for(int m2=0; m2<2048; ++m2) {
            if (sim_left(m1, m2, 1845, t1) && sim_left(m1, m2, 1071, t2)) {
                std::cout << "FOUND LEFT SHIFT! m1=" << m1 << " m2=" << m2 << "\n";
                return 0;
            }
            if (sim_right(m1, m2, 1845, t1) && sim_right(m1, m2, 1071, t2)) {
                std::cout << "FOUND RIGHT SHIFT! m1=" << m1 << " m2=" << m2 << "\n";
                return 0;
            }
        }
    }
    std::cout << "NOT FOUND\n";
    return 0;
}
