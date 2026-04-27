/**
 * LunaNet Gateway 1 - Spreading Code Generator Implementation
 * 
 * This file implements the AFS (AltBOC Forward link Spreading) codes
 * for the LunaNet navigation system.
 */

#include "spreading_codes.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <map>
#include <array>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace lunanet {

    // PRN initialization vectors for G2 register (from CSV data)
    // G2_Initialization_HEX values converted to decimal
    const uint16_t PRN_INIT_G2[] = {
        // PRN 1-10
        0x514, 0x59E, 0x49A, 0x346, 0x788, 0x341, 0x170, 0x7AB, 0x301, 0x32E,
        // PRN 11-20
        0x4DE, 0x290, 0x4ED, 0x506, 0x30E, 0x230, 0x270, 0x380, 0x2DC, 0x5C8,
        // PRN 21-30
        0x61E, 0x720, 0x548, 0x770, 0x32F, 0x600, 0x635, 0x487, 0x730, 0x4BE,
        // PRN 31-40
        0x55F, 0x775, 0x424, 0x603, 0x450, 0x0AB, 0x316, 0x44F, 0x477, 0x76F,
        // PRN 41-50
        0x1F3, 0x189, 0x6BE, 0x6EB, 0x336, 0x650, 0x590, 0x74F, 0x294, 0x03F,
        // PRN 51-60
        0x5AB, 0x4F2, 0x647, 0x494, 0x521, 0x53D, 0x4DF, 0x4A2, 0x2C3, 0x15F,
        // PRN 61-70
        0x3A2, 0x209, 0x329, 0x0A9, 0x532, 0x733, 0x697, 0x6DD, 0x2FC, 0x471,
        // PRN 71-80
        0x428, 0x706, 0x52F, 0x77F, 0x526, 0x7FB, 0x220, 0x13C, 0x65F, 0x7E3,
        // PRN 81-90
        0x254, 0x444, 0x041, 0x542, 0x6EF, 0x4F6, 0x708, 0x45F, 0x654, 0x659,
        // PRN 91-100
        0x486, 0x72A, 0x191, 0x68D, 0x0CD, 0x5FE, 0x604, 0x282, 0x36D, 0x231,
        // PRN 101-110
        0x655, 0x1FF, 0x6FB, 0x41F, 0x6AC, 0x026, 0x298, 0x612, 0x100, 0x2C7,
        // PRN 111-120
        0x534, 0x7BE, 0x7DA, 0x510, 0x39A, 0x786, 0x354, 0x5A4, 0x1A4, 0x529,
        // PRN 121-130
        0x4CE, 0x5F2, 0x280, 0x30D, 0x621, 0x572, 0x455, 0x4C5, 0x51E, 0x676,
        // PRN 131-140
        0x53C, 0x726, 0x410, 0x544, 0x430, 0x3AC, 0x468, 0x389, 0x7DE, 0x3CD,
        // PRN 141-150
        0x7FD, 0x0BB, 0x600, 0x6F9, 0x61A, 0x7EA, 0x798, 0x75A, 0x307, 0x5D1,
        // PRN 151-160
        0x541, 0x689, 0x71E, 0x330, 0x498, 0x377, 0x664, 0x476, 0x6BB, 0x55E,
        // PRN 161-170
        0x340, 0x440, 0x1DE, 0x650, 0x302, 0x32C, 0x2CD, 0x50A, 0x31A, 0x459,
        // PRN 171-180
        0x750, 0x5D2, 0x4F3, 0x779, 0x538, 0x719, 0x146, 0x1E4, 0x5E3, 0x554,
        // PRN 181-190
        0x584, 0x288, 0x226, 0x594, 0x4D4, 0x48C, 0x020, 0x44E, 0x43A, 0x6F0,
        // PRN 191-200
        0x7C5, 0x331, 0x7BD, 0x443, 0x46A, 0x758, 0x357, 0x22C, 0x385, 0x643,
        // PRN 201-210
        0x648, 0x4D3, 0x282, 0x530, 0x760, 0x6E4, 0x485, 0x679, 0x112, 0x2DE
    };

    // G2 delay values in chips (from CSV column G2_Delay_chips)
    const uint16_t PRN_G2_DELAY[] = {
        // PRN 1-10
        1845, 1071, 170, 2035, 1214, 1292, 1284, 1894, 1537, 735,
        // PRN 11-20
        561, 1789, 1453, 196, 1040, 326, 1787, 982, 1030, 1380,
        // PRN 21-30
        1932, 1188, 390, 714, 303, 1001, 707, 1984, 139, 182,
        // PRN 31-40
        1891, 1247, 1434, 2000, 1843, 865, 616, 514, 449, 1173,
        // PRN 41-50
        24, 1383, 1940, 1594, 1765, 752, 145, 1615, 1666, 1372,
        // PRN 51-60
        1634, 1068, 1181, 879, 1153, 1621, 927, 1848, 402, 413,
        // PRN 61-70
        1090, 657, 609, 1547, 370, 271, 1353, 635, 299, 697,
        // PRN 71-80
        152, 678, 1329, 15, 1974, 1884, 1868, 277, 302, 9,
        // PRN 81-90
        603, 1583, 848, 1234, 1568, 510, 1303, 1921, 823, 1187,
        // PRN 91-100
        1299, 824, 672, 2034, 1388, 13, 223, 1840, 1161, 1132,
        // PRN 101-110
        365, 2, 924, 1373, 959, 220, 1542, 188, 264, 453,
        // PRN 111-120
        68, 715, 75, 1095, 938, 1316, 394, 1156, 166, 969,
        // PRN 121-130
        269, 179, 957, 400, 625, 1513, 1796, 100, 1660, 1454,
        // PRN 131-140
        1613, 1064, 844, 518, 320, 661, 2031, 694, 1143, 1167,
        // PRN 141-150
        1885, 833, 1601, 903, 399, 1896, 899, 133, 556, 331,
        // PRN 151-160
        198, 212, 1024, 1070, 1972, 1573, 884, 1177, 1691, 533,
        // PRN 161-170
        480, 751, 447, 734, 973, 857, 1767, 1548, 1876, 614,
        // PRN 171-180
        1017, 1978, 275, 1141, 1252, 1952, 1714, 1067, 557, 522,
        // PRN 181-190
        1159, 545, 1580, 610, 935, 1134, 780, 691, 1038, 1418,
        // PRN 191-200
        295, 916, 1654, 624, 706, 1033, 1633, 790, 1451, 1300,
        // PRN 201-210
        459, 106, 861, 1541, 114, 1381, 1945, 1069, 242, 356
    };

    // Static variables for error handling
    static char last_error[256] = "";
    static const char* version = "1.0.0";

    void initialize_engine() {
        // Initialize any required resources
        std::memset(last_error, 0, sizeof(last_error));
    }

    const char* hello_moon() {
        return "Hello Moon";
    }

    const char* get_last_error() {
        return last_error;
    }

    const char* get_version() {
        return version;
    }

    std::string chips_to_hex(const std::vector<uint8_t>& chips, size_t num_chips) {
        const size_t chip_count = (num_chips == 0) ? chips.size() : std::min(num_chips, chips.size());
        std::string hex_str;
        hex_str.reserve((chip_count + 3) / 4);

        uint8_t current_val = 0;
        for (size_t i = 0; i < chip_count; ++i) {
            current_val = static_cast<uint8_t>((current_val << 1) | (chips[i] & 1u));
            if ((i + 1) % 4 == 0) {
                hex_str += "0123456789ABCDEF"[current_val & 0x0F];
                current_val = 0;
            }
        }

        const size_t remainder = chip_count % 4;
        if (remainder != 0) {
            current_val = static_cast<uint8_t>(current_val << (4 - remainder));
            hex_str += "0123456789ABCDEF"[current_val & 0x0F];
        }

        return hex_str;
    }

    // Helper function to set error message
    static void set_error(const char* msg) {
        std::strncpy(last_error, msg, sizeof(last_error) - 1);
        last_error[sizeof(last_error) - 1] = '\0';
    }

    static bool write_hex_file(const std::filesystem::path& path,
                               const std::vector<std::pair<int, std::string>>& rows,
                               std::string* error_message) {
        std::ofstream out(path, std::ios::binary);
        if (!out) {
            if (error_message) {
                *error_message = "Failed to open output file: " + path.string();
            }
            return false;
        }

        for (const auto& row : rows) {
            out << row.first << ',' << row.second << '\n';
        }

        if (!out.good()) {
            if (error_message) {
                *error_message = "Failed to write output file: " + path.string();
            }
            return false;
        }

        return true;
    }

    bool export_reference_hex_files(const std::string& output_directory, std::string* error_message) {
        try {
            const std::filesystem::path out_dir(output_directory);
            std::filesystem::create_directories(out_dir);

            std::vector<std::pair<int, std::string>> gold_rows;
            std::vector<std::pair<int, std::string>> weil_primary_rows;
            std::vector<std::pair<int, std::string>> weil_tertiary_rows;
            gold_rows.reserve(MAX_PRNS);
            weil_primary_rows.reserve(MAX_PRNS);
            weil_tertiary_rows.reserve(MAX_PRNS);

            for (int prn = 1; prn <= MAX_PRNS; ++prn) {
                gold_rows.emplace_back(prn, chips_to_hex(generate_afs_i(prn)));
                weil_primary_rows.emplace_back(prn, chips_to_hex(generate_weil_primary(prn)));
                weil_tertiary_rows.emplace_back(prn, chips_to_hex(generate_weil_tertiary(prn)));
            }

            if (!write_hex_file(out_dir / "GoldCode2046hex210prns.txt", gold_rows, error_message)) return false;
            if (!write_hex_file(out_dir / "Weil_11cp_hex210prns.txt", weil_primary_rows, error_message)) return false;
            if (!write_hex_file(out_dir / "Weil_1500hex210prns.txt", weil_tertiary_rows, error_message)) return false;
            return true;
        } catch (const std::exception& ex) {
            if (error_message) {
                *error_message = ex.what();
            }
            return false;
        }
    }

    static bool diff_single_file(const std::filesystem::path& reference_path,
                                 const std::filesystem::path& generated_path,
                                 std::ostringstream& report) {
        std::ifstream ref(reference_path, std::ios::binary);
        std::ifstream gen(generated_path, std::ios::binary);
        if (!ref || !gen) {
            report << "Missing file: " << reference_path.string() << " or " << generated_path.string() << '\n';
            return false;
        }

        std::string ref_line;
        std::string gen_line;
        size_t line_no = 0;
        bool matched = true;
        while (true) {
            const bool ref_ok = static_cast<bool>(std::getline(ref, ref_line));
            const bool gen_ok = static_cast<bool>(std::getline(gen, gen_line));
            if (!ref_ok && !gen_ok) break;
            ++line_no;
            if (ref_ok != gen_ok || ref_line != gen_line) {
                matched = false;
                report << "Mismatch at line " << line_no << " in " << reference_path.filename().string() << '\n';
                if (!ref_ok) report << "  reference: <EOF>\n"; else report << "  reference: " << ref_line << '\n';
                if (!gen_ok) report << "  generated: <EOF>\n"; else report << "  generated: " << gen_line << '\n';
                break;
            }
        }

        if (matched) {
            report << "Match: " << reference_path.filename().string() << '\n';
        }
        return matched;
    }

    bool diff_reference_hex_files(const std::string& reference_directory, const std::string& generated_directory, std::string* report) {
        try {
            std::ostringstream summary;
            bool all_match = true;
            const std::filesystem::path ref_dir(reference_directory);
            const std::filesystem::path gen_dir(generated_directory);

            all_match &= diff_single_file(ref_dir / "GoldCode2046hex210prns.txt", gen_dir / "GoldCode2046hex210prns.txt", summary);
            all_match &= diff_single_file(ref_dir / "Weil_11cp_hex210prns.txt", gen_dir / "Weil_11cp_hex210prns.txt", summary);
            all_match &= diff_single_file(ref_dir / "Weil_1500hex210prns.txt", gen_dir / "Weil_1500hex210prns.txt", summary);

            if (report) {
                *report = summary.str();
            }
            return all_match;
        } catch (const std::exception& ex) {
            if (report) {
                *report = ex.what();
            }
            return false;
        }
    }

    // Linear Feedback Shift Register (LFSR) implementation for 11-bit and other lengths
    class LFSR {
    private:
        uint16_t state;
        uint16_t feedback_mask;
        int length;
    
    public:
        LFSR(uint16_t initial_state, uint16_t feedback_mask, int reg_length = 11)
            : state(initial_state), feedback_mask(feedback_mask), length(reg_length) {}
    
        // Generate next bit and update state
        uint8_t next_bit() {
            uint8_t output = (state >> (length - 1)) & 1;

            uint16_t masked_state = state & feedback_mask;
            int feedback = 0;
            for (int i = 0; i < length; i++) {
                feedback ^= (masked_state >> i) & 1;
            }

            state = ((state << 1) & ((1 << length) - 1)) | feedback;

            return output;
        }
    
        // Reset to initial state
        void reset(uint16_t initial_state) {
            state = initial_state;
        }
    
        // Get current state
        uint16_t get_state() const {
            return state;
        }
    };

    // Weil parameter tables (Appendix D: primary, Appendix E: tertiary)
    struct WeilPrimaryParams {
        int weil_index_k;
        int insertion_index_p;
    };

    struct WeilTertiaryParams {
        int weil_index_k;
    };

    // Appendix D: Weil primary code parameters for PRN 1-210
    const std::array<WeilPrimaryParams, 210> WEIL_PRIMARY_PARAMS = {{
        {5111, 412}, {5109, 161}, {5108, 1}, {5106, 303}, {5103, 207}, {5101, 4971}, {5100, 4496}, {5098, 5}, {5095, 4557}, {5094, 485},
        {5093, 253}, {5091, 4676}, {5090, 1}, {5081, 66}, {5080, 4485}, {5069, 282}, {5068, 193}, {5054, 5211}, {5044, 729}, {5027, 4848},
        {5026, 982}, {5014, 5955}, {5004, 9805}, {4980, 670}, {4915, 464}, {4909, 29}, {4893, 429}, {4885, 394}, {4832, 616}, {4824, 9457},
        {4591, 4429}, {3706, 4771}, {5092, 365}, {4986, 9705}, {4965, 9489}, {4920, 4193}, {4917, 9947}, {4858, 824}, {4847, 864}, {4790, 347},
        {4770, 677}, {4318, 6544}, {4126, 6312}, {3961, 9804}, {3790, 278}, {4911, 9461}, {4881, 444}, {4827, 4839}, {4795, 4144}, {4789, 9875},
        {4725, 197}, {4675, 1156}, {4539, 4674}, {4535, 10035}, {4458, 4504}, {4197, 5}, {4096, 9937}, {3484, 430}, {3481, 5}, {3393, 355},
        {3175, 909}, {2360, 6284}, {1852, 6284}, {5065, 9429}, {5063, 77}, {5055, 932}, {5012, 5973}, {4981, 377}, {4952, 10000}, {4934, 951},
        {4932, 6212}, {4786, 686}, {4762, 9352}, {4640, 5999}, {4601, 9912}, {4563, 9620}, {4388, 635}, {3820, 4951}, {3687, 5453}, {5052, 4658},
        {5051, 4800}, {5047, 59}, {5039, 318}, {5015, 571}, {5005, 565}, {4984, 9947}, {4975, 4654}, {4974, 148}, {4972, 3929}, {4962, 293},
        {4913, 178}, {4907, 10142}, {4903, 9683}, {4833, 137}, {4778, 565}, {4721, 35}, {4661, 5949}, {4660, 2}, {4655, 5982}, {4623, 825},
        {4590, 9614}, {4548, 9790}, {4461, 5613}, {4442, 764}, {4347, 660}, {4259, 4870}, {4256, 4950}, {4166, 4881}, {4155, 1151}, {4109, 9977},
        {4100, 5122}, {4023, 10074}, {3998, 4832}, {3979, 77}, {3903, 4698}, {3568, 1002}, {5088, 5549}, {5050, 9606}, {5020, 9228}, {4990, 604},
        {4982, 4678}, {4966, 4854}, {4949, 4122}, {4947, 9471}, {4937, 5026}, {4935, 272}, {4906, 1027}, {4901, 317}, {4872, 691}, {4865, 509},
        {4863, 9708}, {4818, 5033}, {4785, 9938}, {4781, 4314}, {4776, 10140}, {4775, 4790}, {4754, 9823}, {4696, 6093}, {4690, 469}, {4658, 1215},
        {4607, 799}, {4599, 756}, {4596, 9994}, {4530, 4843}, {4524, 5271}, {4451, 9661}, {4441, 6255}, {4396, 5203}, {4340, 203}, {4335, 10070},
        {4296, 30}, {4267, 103}, {4168, 5692}, {4149, 32}, {4097, 9826}, {4061, 76}, {3989, 59}, {3966, 6831}, {3789, 958}, {3775, 1471},
        {3622, 10070}, {3523, 553}, {3515, 5487}, {3492, 55}, {3345, 208}, {3235, 645}, {3169, 5268}, {3157, 1873}, {3082, 427}, {3072, 367},
        {3032, 1404}, {3030, 5652}, {4582, 5}, {4595, 368}, {4068, 451}, {4871, 9595}, {4514, 1030}, {4439, 1324}, {4122, 692}, {4948, 9819},
        {4774, 4520}, {3923, 9911}, {3411, 278}, {4745, 642}, {4195, 6330}, {4897, 5508}, {3047, 1872}, {4185, 5445}, {4354, 10131}, {5077, 422},
        {4042, 4918}, {2111, 787}, {4311, 9864}, {5024, 9753}, {4352, 9859}, {4678, 328}, {5034, 1}, {5085, 4733}, {3646, 164}, {4868, 135},
        {3668, 174}, {4211, 132}, {2883, 538}, {2850, 176}, {2815, 198}, {2542, 595}, {2492, 574}, {2376, 321}, {2036, 596}, {1920, 491}
    }};

    // Appendix E: Weil tertiary code parameters for PRN 1-210
    const std::array<WeilTertiaryParams, 210> WEIL_TERTIARY_PARAMS = {{
        {1}, {229}, {237}, {241}, {253}, {254}, {255}, {256}, {257}, {267},
        {276}, {283}, {301}, {319}, {327}, {328}, {333}, {334}, {335}, {339},
        {340}, {346}, {347}, {350}, {354}, {356}, {357}, {361}, {364}, {365},
        {366}, {368}, {373}, {378}, {381}, {382}, {383}, {384}, {386}, {387},
        {389}, {390}, {394}, {397}, {398}, {400}, {401}, {407}, {408}, {414},
        {415}, {416}, {426}, {431}, {432}, {433}, {436}, {437}, {438}, {439},
        {440}, {441}, {447}, {448}, {449}, {450}, {451}, {459}, {460}, {461},
        {463}, {467}, {468}, {469}, {471}, {474}, {475}, {477}, {480}, {481},
        {485}, {487}, {488}, {489}, {490}, {491}, {492}, {495}, {496}, {498},
        {500}, {502}, {507}, {509}, {510}, {513}, {515}, {517}, {520}, {521},
        {524}, {525}, {526}, {527}, {528}, {529}, {531}, {533}, {538}, {540},
        {542}, {543}, {544}, {549}, {551}, {552}, {553}, {554}, {555}, {556},
        {557}, {562}, {568}, {570}, {573}, {574}, {575}, {576}, {577}, {578},
        {579}, {580}, {582}, {586}, {591}, {592}, {594}, {595}, {596}, {597},
        {598}, {599}, {601}, {604}, {605}, {606}, {607}, {608}, {609}, {610},
        {612}, {616}, {617}, {618}, {619}, {621}, {622}, {627}, {628}, {631},
        {633}, {635}, {638}, {639}, {640}, {645}, {647}, {648}, {649}, {650},
        {651}, {654}, {656}, {658}, {660}, {661}, {662}, {665}, {668}, {669},
        {671}, {674}, {675}, {676}, {678}, {680}, {682}, {683}, {684}, {686},
        {687}, {688}, {689}, {690}, {691}, {697}, {698}, {702}, {705}, {707},
        {710}, {716}, {717}, {718}, {720}, {722}, {723}, {725}, {726}, {729}
    }};

    // Legendre symbol computation for Weil code generation
    static int legendre_symbol(int a, int p) {
        if (a == 0) return 0;
        if (p == 2) return a & 1; // For p=2, return a mod 2
    
        // Euler's criterion: a^((p-1)/2) mod p
        long long result = 1;
        long long base = a % p;
        long long exponent = (p - 1) / 2;
    
        while (exponent > 0) {
            if (exponent & 1) {
                result = (result * base) % p;
            }
            base = (base * base) % p;
            exponent >>= 1;
        }
    
        if (result == p - 1) return -1;
        return result;
    }

    // Generate Gold code for a given PRN (11-bit LFSR, 2046-chip output)
    // Per LSIS-AFS Appendix C: uses 11-bit generators, PRN-specific delay already baked into PRN_INIT_G2
    std::vector<uint8_t> generate_gold_code(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }
    
        // G1 uses fixed all-1s initialization for 11 bits
        LFSR g1(G1_INIT, G1_TAP_FEEDBACK, LFSR_LENGTH);
    
        // G2 uses PRN-specific initialization (delay is already baked into the CSV init value)
        // Do NOT apply additional G2_DELAY shift - PRN_INIT_G2 is the pre-delayed state
        uint16_t g2_init = PRN_INIT_G2[prn - 1];
        LFSR g2(g2_init, G2_TAP_FEEDBACK, LFSR_LENGTH);
    
        std::vector<uint8_t> gold_code;
        gold_code.reserve(GOLD_CODE_LENGTH);
    
        // Generate 2046 chips (2^11 - 1 shortened cycle)
        for (int i = 0; i < GOLD_CODE_LENGTH; i++) {
            uint8_t g1_bit = g1.next_bit();
            uint8_t g2_bit = g2.next_bit();
            gold_code.push_back(g1_bit ^ g2_bit);
        }
    
        return gold_code;
    }

    // Generate Legendre sequence for a given prime
    std::vector<uint8_t> generate_legendre_sequence(int prime) {
        std::vector<uint8_t> legendre;
        legendre.reserve(prime);
    
        for (int n = 0; n < prime; n++) {
            int symbol = legendre_symbol(n, prime);
            // Convert Legendre symbol to binary: +1 -> 1, (0 or -1) -> 0
            legendre.push_back((symbol == 1) ? 1 : 0);
        }
    
        return legendre;
    }

    // Generate PRN-specific Weil primary code (Appendix D)
    // W(t;k) = L(t) XOR L((t+k) mod 10223), with expansion [0,1,1,0,1,0,0] inserted at index p
    std::vector<uint8_t> generate_weil_primary(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }
    
        // Get PRN-specific Weil parameters
        int k = WEIL_PRIMARY_PARAMS[prn - 1].weil_index_k;
        int p = WEIL_PRIMARY_PARAMS[prn - 1].insertion_index_p;
    
        // Generate Legendre sequence L(t) for prime 10223
        std::vector<uint8_t> legendre = generate_legendre_sequence(WEIL_PRIMARY_PRIME);
    
        // Compute Weil code: W(t;k) = L(t) XOR L((t+k) mod 10223)
        std::vector<uint8_t> weil;
        weil.reserve(WEIL_PRIMARY_PRIME + EXPANSION_LENGTH);
    
        for (int t = 0; t < WEIL_PRIMARY_PRIME; t++) {
            uint8_t bit = legendre[t] ^ legendre[(t + k) % WEIL_PRIMARY_PRIME];
        
            // Insert expansion at index p
            if (t == p) {
                weil.push_back(legendre[t] ^ legendre[(t + k) % WEIL_PRIMARY_PRIME]);
                // Insert 7-bit expansion: [0, 1, 1, 0, 1, 0, 0]
                weil.push_back(0);
                weil.push_back(1);
                weil.push_back(1);
                weil.push_back(0);
                weil.push_back(1);
                weil.push_back(0);
                weil.push_back(0);
            } else {
                weil.push_back(bit);
            }
        }
    
        return weil;
    }

    // Generate PRN-specific Weil tertiary code (Appendix E)
    // W(t;k) = L(t) XOR L((t+k) mod 1499), with 0 appended at end
    std::vector<uint8_t> generate_weil_tertiary(int prn) {
        if (prn < 1 || prn > MAX_PRNS) {
            set_error("PRN must be between 1 and 210");
            return {};
        }
    
        // Get PRN-specific Weil parameter
        int k = WEIL_TERTIARY_PARAMS[prn - 1].weil_index_k;
    
        // Generate Legendre sequence L(t) for prime 1499
        std::vector<uint8_t> legendre = generate_legendre_sequence(WEIL_TERTIARY_PRIME);
    
        // Compute Weil code: W(t;k) = L(t) XOR L((t+k) mod 1499)
        std::vector<uint8_t> weil;
        weil.reserve(WEIL_TERTIARY_LENGTH);
    
        for (int t = 0; t < WEIL_TERTIARY_PRIME; t++) {
            uint8_t bit = legendre[t] ^ legendre[(t + k) % WEIL_TERTIARY_PRIME];
            weil.push_back(bit);
        }
    
        // Append final 0 per spec
        weil.push_back(0);
    
        return weil;
    }

    // Generate the complete AFS-I data channel (Gold codes, 2046 chips)
    std::vector<uint8_t> generate_afs_i(int prn) {
        return generate_gold_code(prn);
    }

    // Generate the complete AFS-Q pilot channel (Weil codes with XOR interleaving per spec)
    std::vector<uint8_t> generate_afs_q(int prn, int variant) {
        std::vector<uint8_t> primary = generate_weil_primary(prn);
        std::vector<uint8_t> tertiary = generate_weil_tertiary(prn);

        uint8_t secondary[4];
        switch (variant) {
            case 1: secondary[0] = 0; secondary[1] = 1; secondary[2] = 1; secondary[3] = 1; break;
            case 2: secondary[0] = 1; secondary[1] = 0; secondary[2] = 1; secondary[3] = 1; break;
            case 3: secondary[0] = 1; secondary[1] = 1; secondary[2] = 0; secondary[3] = 1; break;
            case 0:
            default: secondary[0] = 1; secondary[1] = 1; secondary[2] = 1; secondary[3] = 0; break;
        }

        std::vector<uint8_t> afs_q;
        afs_q.reserve(primary.size());

        for (size_t i = 0; i < primary.size(); ++i) {
            const uint8_t p_chip = primary[i];
            const uint8_t t_chip = tertiary[i % tertiary.size()];
            const uint8_t s_chip = secondary[(i / tertiary.size()) % 4u];
            afs_q.push_back(static_cast<uint8_t>(p_chip ^ t_chip ^ s_chip));
        }

        return afs_q;
    }

    // Generate all spreading codes for all 210 PRNs (Phase 1B batch generator)
    std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> generate_all_spreading_codes() {
        std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> all_codes;
    
        for (int prn = 1; prn <= MAX_PRNS; ++prn) {
            std::vector<uint8_t> afs_i = generate_afs_i(prn);
            std::vector<uint8_t> afs_q = generate_afs_q(prn, 0);  // Using variant 0 (S0)
            all_codes[prn] = std::make_pair(afs_i, afs_q);
        }
    
        return all_codes;
    }
} // namespace lunanet 