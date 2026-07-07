// LSIS-AFS CLI tool for interoperability testing.
//
// Subcommands:
//   generate-codes  — Generate all 210 Gold primary spreading codes as hex.
//   encode          — Encode a navigation frame and optionally modulate to I/Q.
//   version         — Print version string.
//
// Conforms to the CLI contract specified in the CCSDS 235.1 & LSIS-AFS
// Mid-Project Workshop programme.

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "spreading_codes.h"
#include "gateway3/frame_assembler.h"
#include "gateway3/frame_exporter.h"
#include "gateway4/bpsk_modulator.h"
#include "gateway4/iq_generator.h"
#include "gateway4/signal_config.h"
#include "gateway4/signal_exporter.h"

namespace {

// ── Argument helpers ────────────────────────────────────────────────────────

struct Args {
    int argc;
    char** argv;
    int pos = 0;
};

bool HasNext(const Args& a) { return a.pos < a.argc; }

const char* Peek(const Args& a) {
    return (a.pos < a.argc) ? a.argv[a.pos] : nullptr;
}

const char* Next(Args& a) {
    return (a.pos < a.argc) ? a.argv[a.pos++] : nullptr;
}

bool Match(Args& a, const char* flag) {
    if (a.pos < a.argc && std::strcmp(a.argv[a.pos], flag) == 0) {
        ++a.pos;
        return true;
    }
    return false;
}

bool GetString(Args& a, const char* flag, std::string& out) {
    if (a.pos + 1 < a.argc && std::strcmp(a.argv[a.pos], flag) == 0) {
        out = a.argv[a.pos + 1];
        a.pos += 2;
        return true;
    }
    return false;
}

bool GetInt(Args& a, const char* flag, int& out) {
    std::string s;
    if (GetString(a, flag, s)) {
        char* end = nullptr;
        long v = std::strtol(s.c_str(), &end, 10);
        if (end == s.c_str() || *end != '\0') {
            std::cerr << "error: invalid integer for " << flag << ": " << s << "\n";
            return false;
        }
        out = static_cast<int>(v);
        return true;
    }
    return false;
}

// ── Config path resolution ──────────────────────────────────────────────────

std::string FindConfigPath(const std::string& explicit_path) {
    if (!explicit_path.empty()) return explicit_path;

    // Walk up from the executable (or cwd) looking for config/spreading_codes_config.ini
    namespace fs = std::filesystem;
    fs::path search = fs::current_path();
    for (int depth = 0; depth < 5; ++depth) {
        fs::path candidate = search / "config" / "spreading_codes_config.ini";
        if (fs::exists(candidate)) return candidate.string();
        if (!search.has_parent_path() || search == search.parent_path()) break;
        search = search.parent_path();
    }
    return "config/spreading_codes_config.ini";
}

std::string FindAnnex3CsvDir(const std::string& explicit_path) {
    if (!explicit_path.empty()) return explicit_path;

    namespace fs = std::filesystem;
    fs::path search = fs::current_path();
    for (int depth = 0; depth < 5; ++depth) {
        fs::path candidate = search / "Validation" / "annex3" / "csv";
        if (fs::is_directory(candidate)) return candidate.string();
        if (!search.has_parent_path() || search == search.parent_path()) break;
        search = search.parent_path();
    }
    return "Validation/annex3/csv";
}

// ── Hex string to bytes ─────────────────────────────────────────────────────

std::vector<uint8_t> ParseHexPayload(const std::string& hex) {
    std::vector<uint8_t> bits;
    for (char c : hex) {
        int v = -1;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'A' && c <= 'F') v = 10 + (c - 'A');
        else if (c >= 'a' && c <= 'f') v = 10 + (c - 'a');
        else continue;
        for (int b = 3; b >= 0; --b) {
            bits.push_back(static_cast<uint8_t>((v >> b) & 1));
        }
    }
    return bits;
}

enum class CodeFamily {
    Gold,
    WeilPrimary,
    WeilTertiary,
    All,
};

bool ParseCodeFamily(const std::string& text, CodeFamily& out) {
    if (text.empty() || text == "gold") {
        out = CodeFamily::Gold;
        return true;
    }
    if (text == "weil-primary" || text == "weil_primary" || text == "weil") {
        out = CodeFamily::WeilPrimary;
        return true;
    }
    if (text == "weil-tertiary" || text == "weil_tertiary" || text == "tertiary") {
        out = CodeFamily::WeilTertiary;
        return true;
    }
    if (text == "all") {
        out = CodeFamily::All;
        return true;
    }
    return false;
}

bool WriteCodeFamily(std::ostream& out_stream, CodeFamily family, std::string& error) {
    for (int prn = 1; prn <= lunanet::MAX_PRNS; ++prn) {
        std::vector<uint8_t> code;
        switch (family) {
            case CodeFamily::Gold:
                code = lunanet::generate_gold_code(prn);
                break;
            case CodeFamily::WeilPrimary:
                code = lunanet::generate_weil_primary(prn);
                break;
            case CodeFamily::WeilTertiary:
                code = lunanet::generate_weil_tertiary(prn);
                break;
            case CodeFamily::All:
                error = "Internal error: CodeFamily::All is not valid for WriteCodeFamily";
                return false;
        }

        if (code.empty()) {
            error = "Failed to generate code for PRN " + std::to_string(prn) +
                    ": " + lunanet::get_last_error();
            return false;
        }

        out_stream << lunanet::chips_to_hex(code, 0) << "\n";
    }

    return true;
}

const char* CodeFamilyFileName(CodeFamily family) {
    switch (family) {
        case CodeFamily::Gold:
            return "gold_codes.txt";
        case CodeFamily::WeilPrimary:
            return "weil_primary_codes.txt";
        case CodeFamily::WeilTertiary:
            return "weil_tertiary_codes.txt";
        case CodeFamily::All:
            return "all_codes.txt";
    }
    return "codes.txt";
}

// ── Subcommand: generate-codes ──────────────────────────────────────────────

int CmdGenerateCodes(Args& a) {
    std::string output;
    std::string config;
    std::string codes = "gold";

    while (HasNext(a)) {
        if (GetString(a, "--output", output)) continue;
        if (GetString(a, "--config", config)) continue;
        if (GetString(a, "--codes", codes)) continue;
        std::cerr << "error: unknown option: " << Peek(a) << "\n";
        return 1;
    }

    config = FindConfigPath(config);
    lunanet::initialize_engine();
    std::string err;
    if (!lunanet::load_spreading_code_config(config, &err)) {
        std::cerr << "error: failed to load config: " << err << "\n";
        return 1;
    }

    CodeFamily family;
    if (!ParseCodeFamily(codes, family)) {
        std::cerr << "error: invalid --codes value: " << codes << "\n"
                  << "       expected one of: gold, weil-primary, weil-tertiary, all\n";
        return 1;
    }

    if (family == CodeFamily::All) {
        namespace fs = std::filesystem;
        const fs::path output_dir = output.empty() ? fs::path(".") : fs::path(output);

        std::error_code fs_error;
        bool dir_exists = fs::exists(output_dir, fs_error);
        if (fs_error) {
            std::cerr << "error: cannot check output path: "
                      << output_dir.string() << ": " << fs_error.message() << "\n";
            return 1;
        }
        if (dir_exists) {
            bool is_dir = fs::is_directory(output_dir, fs_error);
            if (fs_error) {
                std::cerr << "error: cannot check output path: "
                          << output_dir.string() << ": " << fs_error.message() << "\n";
                return 1;
            }
            if (!is_dir) {
                std::cerr << "error: for --codes all, --output must be a directory: "
                          << output_dir.string() << "\n";
                return 1;
            }
        } else {
            fs::create_directories(output_dir, fs_error);
            if (fs_error) {
                std::cerr << "error: cannot create output directory: "
                          << output_dir.string() << ": " << fs_error.message() << "\n";
                return 1;
            }
        }

        for (CodeFamily current_family : {CodeFamily::Gold, CodeFamily::WeilPrimary, CodeFamily::WeilTertiary}) {
            const fs::path file_path = output_dir / CodeFamilyFileName(current_family);
            std::ofstream file_stream(file_path);
            if (!file_stream) {
                std::cerr << "error: cannot open output file: " << file_path.string() << "\n";
                return 1;
            }
            if (!WriteCodeFamily(file_stream, current_family, err)) {
                std::cerr << "error: " << err << "\n";
                return 1;
            }
        }
        return 0;
    }

    std::ostream* out_stream = &std::cout;
    std::ofstream file_stream;
    if (!output.empty()) {
        file_stream.open(output);
        if (!file_stream) {
            std::cerr << "error: cannot open output file: " << output << "\n";
            return 1;
        }
        out_stream = &file_stream;
    }

    if (!WriteCodeFamily(*out_stream, family, err)) {
        std::cerr << "error: " << err << "\n";
        return 1;
    }

    return 0;
}

// ── Subcommand: encode ──────────────────────────────────────────────────────

void PrintEncodeUsage() {
    std::cerr
        << "usage: goon encode --format <frame|iq32> --prn <N>\n"
        << "       --fid <N> --toi <N> --wn <N> --itow <N>\n"
        << "       [--ced <hex>] [--rate <hz>] [--output <path>]\n"
        << "       [--config <path>] [--csv-dir <path>]\n";
}

int CmdEncode(Args& a) {
    std::string format;
    int prn = -1, fid = -1, toi = -1, wn = -1, itow = -1;
    int rate = lunanet::gateway4::kAfsIChipRateHz;  // default: 1.023 MHz
    std::string ced_hex;
    std::string output;
    std::string codes;
    std::string config;
    std::string csv_dir;

    while (HasNext(a)) {
        if (GetString(a, "--format", format)) continue;
        if (GetInt(a, "--prn", prn)) continue;
        if (GetInt(a, "--fid", fid)) continue;
        if (GetInt(a, "--toi", toi)) continue;
        if (GetInt(a, "--wn", wn)) continue;
        if (GetInt(a, "--itow", itow)) continue;
        if (GetInt(a, "--rate", rate)) continue;
        if (GetString(a, "--ced", ced_hex)) continue;
        if (GetString(a, "--output", output)) continue;
        if (GetString(a, "--codes", codes)) {
            std::cerr << "error: --codes is not supported for encode (encode always uses Gold); got: "
                      << codes << "\n";
            return 1;
        }
        if (GetString(a, "--config", config)) continue;
        if (GetString(a, "--csv-dir", csv_dir)) continue;
        std::cerr << "error: unknown option: " << Peek(a) << "\n";
        PrintEncodeUsage();
        return 1;
    }

    if (format.empty()) {
        std::cerr << "error: --format is required (frame or iq32)\n";
        PrintEncodeUsage();
        return 1;
    }
    if (format != "frame" && format != "iq32") {
        std::cerr << "error: --format must be 'frame' or 'iq32', got: " << format << "\n";
        return 1;
    }
    if (prn < 1 || prn > lunanet::MAX_PRNS) {
        std::cerr << "error: --prn must be 1–" << lunanet::MAX_PRNS << "\n";
        return 1;
    }
    if (fid < 0 || fid > 3) {
        std::cerr << "error: --fid must be 0–3\n";
        return 1;
    }
    if (toi < 0 || toi > 99) {
        std::cerr << "error: --toi must be 0–99\n";
        return 1;
    }
    if (wn < 0 || wn > 8191) {
        std::cerr << "error: --wn must be 0–8191\n";
        return 1;
    }
    if (itow < 0 || itow > 511) {
        std::cerr << "error: --itow must be 0–511\n";
        return 1;
    }

    // ── Initialize engine and load LDPC matrices ───────────────────────────
    config = FindConfigPath(config);
    lunanet::initialize_engine();
    std::string err;
    if (!lunanet::load_spreading_code_config(config, &err)) {
        std::cerr << "error: failed to load config: " << err << "\n";
        return 1;
    }

    csv_dir = FindAnnex3CsvDir(csv_dir);
    lunanet::gateway3::FrameMatrices matrices;
    if (!lunanet::gateway3::LoadFrameMatrices(csv_dir, &matrices, &err)) {
        std::cerr << "error: failed to load LDPC matrices from " << csv_dir
                  << ": " << err << "\n";
        return 1;
    }

    // ── Build frame input ──────────────────────────────────────────────────
    lunanet::gateway3::FrameInput frame_input;
    frame_input.fid = static_cast<uint8_t>(fid);
    frame_input.toi = static_cast<uint8_t>(toi);

    frame_input.sb2.wn = static_cast<uint16_t>(wn);
    frame_input.sb2.itow = static_cast<uint16_t>(itow);
    frame_input.sb2.toi = static_cast<uint8_t>(toi);
    if (!ced_hex.empty()) {
        frame_input.sb2.payload_bits = ParseHexPayload(ced_hex);
    }
    // SB3 and SB4 default to type=0, empty payload (spare-filled by builders).
    frame_input.sb3.type = 0;
    frame_input.sb4.type = 0;

    // ── Assemble frame ─────────────────────────────────────────────────────
    std::cerr << "Assembling frame: PRN=" << prn << " FID=" << fid
              << " TOI=" << toi << " WN=" << wn << " ITOW=" << itow << "\n";

    auto frame = lunanet::gateway3::AssembleFrame(frame_input, matrices, &err);
    if (frame.empty()) {
        std::cerr << "error: frame assembly failed: " << err << "\n";
        return 1;
    }

    std::cerr << "Frame assembled: " << frame.size() << " symbols\n";

    // ── Output frame ───────────────────────────────────────────────────────
    if (format == "frame") {
        if (output.empty()) output = "frame.bin";
        if (!lunanet::gateway3::ExportFrameRaw(frame, output, &err)) {
            std::cerr << "error: frame export failed: " << err << "\n";
            return 1;
        }
        std::cerr << "Frame written to " << output << " ("
                  << frame.size() << " bytes)\n";
        return 0;
    }

    // ── format == "iq32": full signal generation pipeline ──────────────────
    if (output.empty()) output = "signal.iq32";

    // Step 1: Generate spreading codes for this PRN.
    auto gold_code = lunanet::generate_gold_code(prn);
    if (gold_code.empty()) {
        std::cerr << "error: Gold code generation failed: "
                  << lunanet::get_last_error() << "\n";
        return 1;
    }

    // AFS-I: data-modulate the frame symbols onto the Gold primary code.
    // Each of the 6000 symbols gets XOR'd across one full 2046-chip epoch.
    auto afs_i_chips = lunanet::gateway4::ModulateAfsIData(gold_code, frame, &err);
    if (afs_i_chips.empty()) {
        std::cerr << "error: AFS-I modulation failed: " << err << "\n";
        return 1;
    }
    std::cerr << "AFS-I: " << afs_i_chips.size() << " chips ("
              << afs_i_chips.size() / lunanet::gateway4::kAfsIPrimaryChips
              << " symbols × " << lunanet::gateway4::kAfsIPrimaryChips << " chips)\n";

    // AFS-Q: generate tiered pilot code for the same time span.
    // The Q channel runs at 5× the I chip rate, so we need 5× the I chips.
    const size_t afs_q_chip_count =
        afs_i_chips.size() * static_cast<size_t>(lunanet::gateway4::kQOverIChipRatio);
    auto afs_q_chips = lunanet::generate_afs_q(prn, afs_q_chip_count);
    if (afs_q_chips.empty()) {
        std::cerr << "error: AFS-Q code generation failed: "
                  << lunanet::get_last_error() << "\n";
        return 1;
    }
    std::cerr << "AFS-Q: " << afs_q_chips.size() << " chips\n";

    // Step 2: Generate complex baseband I/Q.
    lunanet::gateway4::IqConfig iq_config;
    iq_config.sample_rate_hz = rate;
    auto signal = lunanet::gateway4::GenerateIq(afs_i_chips, afs_q_chips, iq_config, &err);
    if (signal.i.empty()) {
        std::cerr << "error: I/Q generation failed: " << err << "\n";
        return 1;
    }
    std::cerr << "I/Q signal: " << signal.i.size() << " samples at "
              << signal.sample_rate_hz << " Hz\n";

    // Step 3: Export as interleaved float32 LE binary.
    if (!lunanet::gateway4::ExportIqBinary(signal, output, &err)) {
        std::cerr << "error: I/Q export failed: " << err << "\n";
        return 1;
    }

    const size_t file_bytes = signal.i.size() * 2 * sizeof(float);
    std::cerr << "Signal written to " << output << " ("
              << file_bytes << " bytes, "
              << (file_bytes / (1024 * 1024)) << " MB)\n";
    return 0;
}

// ── Usage ───────────────────────────────────────────────────────────────────

void PrintUsage(const char* prog) {
    std::cerr
        << "LSIS-AFS CLI tool v" << lunanet::get_version() << "\n\n"
        << "usage: " << prog << " <command> [options]\n\n"
        << "commands:\n"
        << "  generate-codes  Generate 210 spreading codes (Gold/Weil/Tertiary)\n"
        << "  encode          Encode navigation frame / generate I/Q signal\n"
        << "  version         Print version\n\n"
        << "examples:\n"
        << "  " << prog << " generate-codes --output codes.txt\n"
        << "  " << prog << " generate-codes --codes gold --output gold_codes.txt\n"
        << "  " << prog << " generate-codes --codes all  --output generated/\n"
        << "  " << prog << " encode --format frame --prn 1 --fid 0 --toi 42 --wn 100 --itow 250\n"
        << "  " << prog << " encode --format iq32  --prn 1 --fid 0 --toi 42 --wn 100 --itow 250\n";
}

}  // namespace

// ── Entry point ─────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    Args a{argc, argv, 1};
    const char* cmd = Next(a);

    if (std::strcmp(cmd, "version") == 0) {
        std::cout << "goon " << lunanet::get_version() << "\n";
        return 0;
    }

    if (std::strcmp(cmd, "generate-codes") == 0) {
        return CmdGenerateCodes(a);
    }

    if (std::strcmp(cmd, "encode") == 0) {
        return CmdEncode(a);
    }

    if (std::strcmp(cmd, "help") == 0 || std::strcmp(cmd, "--help") == 0 ||
        std::strcmp(cmd, "-h") == 0) {
        PrintUsage(argv[0]);
        return 0;
    }

    std::cerr << "error: unknown command: " << cmd << "\n";
    PrintUsage(argv[0]);
    return 1;
}
