#include "bpsk_modulator.h"

#include "signal_config.h"

namespace lunanet::gateway4 {

float BpskMap(uint8_t logic_level) {
    return (logic_level & 1u) ? kBpskLevelLogic1 : kBpskLevelLogic0;
}

std::vector<float> BpskModulate(const std::vector<uint8_t>& chips) {
    std::vector<float> samples;
    samples.reserve(chips.size());
    for (const uint8_t c : chips) {
        samples.push_back(BpskMap(c));
    }
    return samples;
}

std::vector<uint8_t> ModulateAfsIData(const std::vector<uint8_t>& primary_code,
                                      const std::vector<uint8_t>& data_symbols,
                                      std::string* error_message) {
    if (static_cast<int>(primary_code.size()) != kAfsIPrimaryChips) {
        if (error_message) {
            *error_message = "AFS-I primary code length " +
                std::to_string(primary_code.size()) + " (expected " +
                std::to_string(kAfsIPrimaryChips) + ")";
        }
        return {};
    }
    if (data_symbols.empty()) {
        if (error_message) *error_message = "No data symbols provided";
        return {};
    }

    std::vector<uint8_t> chips;
    chips.reserve(data_symbols.size() * static_cast<size_t>(kAfsIPrimaryChips));

    // LSIS-160: one full primary code epoch per data symbol; the symbol bit
    // modulo-2 adds onto every chip of that epoch.
    for (const uint8_t symbol : data_symbols) {
        const uint8_t s = symbol & 1u;
        for (const uint8_t chip : primary_code) {
            chips.push_back(static_cast<uint8_t>(chip ^ s));
        }
    }

    return chips;
}

}  // namespace lunanet::gateway4
