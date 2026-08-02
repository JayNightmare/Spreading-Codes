#include "signal_exporter.h"

#include <cstdio>
#include <iostream>
#include <string>

namespace {

bool TestStandardFormatRoundTrip() {
    lunanet::gateway4::IqSignal signal;
    signal.sample_rate_hz = 1023000;
    for (int n = 0; n < 100; ++n) {
        signal.i.push_back((n % 2 == 0) ? 1.0f : -1.0f);
        signal.q.push_back((n % 3 == 0) ? 1.0f : -1.0f);
    }

    const std::string path = "test_iq_standard.tmp.iq";
    std::string error;
    if (!lunanet::gateway4::ExportIqBinaryStandard(signal, /*prn=*/7, path, &error)) {
        std::cerr << "FAIL [export]: " << error << "\n";
        return false;
    }

    lunanet::gateway4::IqSignal read_back;
    lunanet::gateway4::IqFileHeader header;
    if (!lunanet::gateway4::ImportIqBinaryStandard(path, &read_back, &header, &error)) {
        std::cerr << "FAIL [import]: " << error << "\n";
        std::remove(path.c_str());
        return false;
    }
    std::remove(path.c_str());

    bool ok = true;
    if (header.prn != 7) {
        std::cerr << "FAIL: prn mismatch, got " << header.prn << "\n";
        ok = false;
    }
    if (header.sample_rate_hz != 1023000.0) {
        std::cerr << "FAIL: sample_rate_hz mismatch, got " << header.sample_rate_hz << "\n";
        ok = false;
    }
    if (read_back.i.size() != signal.i.size() || read_back.q.size() != signal.q.size()) {
        std::cerr << "FAIL: sample count mismatch\n";
        ok = false;
    }
    for (size_t n = 0; n < signal.i.size() && ok; ++n) {
        if (read_back.i[n] != signal.i[n] || read_back.q[n] != signal.q[n]) {
            std::cerr << "FAIL: sample " << n << " mismatch\n";
            ok = false;
        }
    }
    return ok;
}

bool TestRejectsBadMagic() {
    // Write a file whose header lacks the "LSISIQ\0\0" magic entirely and
    // confirm the importer rejects it rather than misinterpreting garbage.
    const std::string path = "test_iq_bad_magic.tmp.iq";
    {
        std::string junk(128 + 8, 'X');
        FILE* f = std::fopen(path.c_str(), "wb");
        if (!f) {
            std::cerr << "FAIL: could not create fixture file " << path << "\n";
            return false;
        }
        std::fwrite(junk.data(), 1, junk.size(), f);
        std::fclose(f);
    }

    lunanet::gateway4::IqSignal signal;
    lunanet::gateway4::IqFileHeader header;
    std::string error;
    const bool imported =
        lunanet::gateway4::ImportIqBinaryStandard(path, &signal, &header, &error);
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
        std::cout << "PASS: standardized I/Q file header/samples round-trip exactly\n";
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
