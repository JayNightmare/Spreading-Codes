#include "frame_exporter.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool TestStandardFormatRoundTrip() {
    std::vector<uint8_t> frame(6000);
    for (size_t i = 0; i < frame.size(); ++i) {
        frame[i] = static_cast<uint8_t>(i % 2);
    }

    const std::string path = "test_frame_standard.tmp.bin";
    std::string error;
    if (!lunanet::gateway3::ExportFrameStandard(frame, /*prn=*/12, /*timestamp_unix=*/1234567890ULL,
                                                path, &error)) {
        std::cerr << "FAIL [export]: " << error << "\n";
        return false;
    }

    std::vector<uint8_t> read_back;
    lunanet::gateway3::FrameFileHeader header;
    if (!lunanet::gateway3::ImportFrameStandard(path, &read_back, &header, &error)) {
        std::cerr << "FAIL [import]: " << error << "\n";
        std::remove(path.c_str());
        return false;
    }
    std::remove(path.c_str());

    bool ok = true;
    if (header.prn != 12) {
        std::cerr << "FAIL: prn mismatch, got " << header.prn << "\n";
        ok = false;
    }
    if (header.frame_length != 6000) {
        std::cerr << "FAIL: frame_length mismatch, got " << header.frame_length << "\n";
        ok = false;
    }
    if (header.timestamp_unix != 1234567890ULL) {
        std::cerr << "FAIL: timestamp mismatch, got " << header.timestamp_unix << "\n";
        ok = false;
    }
    if (read_back != frame) {
        std::cerr << "FAIL: symbol payload does not match the original frame\n";
        ok = false;
    }
    return ok;
}

bool TestRejectsBadMagic() {
    const std::string path = "test_frame_bad_magic.tmp.bin";
    {
        std::string junk(64 + 10, 'X');
        FILE* f = std::fopen(path.c_str(), "wb");
        std::fwrite(junk.data(), 1, junk.size(), f);
        std::fclose(f);
    }

    std::vector<uint8_t> frame;
    lunanet::gateway3::FrameFileHeader header;
    std::string error;
    const bool imported = lunanet::gateway3::ImportFrameStandard(path, &frame, &header, &error);
    std::remove(path.c_str());

    if (imported || error.empty()) {
        std::cerr << "FAIL: expected magic-mismatch rejection with an error message\n";
        return false;
    }
    return true;
}

}  // namespace

int main() {
    bool ok = true;

    if (TestStandardFormatRoundTrip()) {
        std::cout << "PASS: standardized frame file header/payload round-trip exactly\n";
    } else {
        ok = false;
    }

    if (TestRejectsBadMagic()) {
        std::cout << "PASS: a file with the wrong magic bytes is rejected\n";
    } else {
        ok = false;
    }

    return ok ? 0 : 1;
}
