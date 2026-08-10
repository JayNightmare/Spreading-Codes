#include "weil_code_generator.h"

#include <array>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace lunanet::gateway1 {

namespace {

bool IsPrime(int n) {
    if (n < 2) {
        return false;
    }
    if (n % 2 == 0) {
        return n == 2;
    }
    for (int divisor = 3; divisor <= n / divisor; divisor += 2) {
        if (n % divisor == 0) {
            return false;
        }
    }
    return true;
}

int LegendreSymbol(int value, int prime) {
    if (value == 0) {
        return 0;
    }

    long long result = 1;
    long long base = value % prime;
    long long exponent = (prime - 1) / 2;

    while (exponent > 0) {
        if ((exponent & 1LL) != 0) {
            result = (result * base) % prime;
        }
        base = (base * base) % prime;
        exponent >>= 1LL;
    }

    return (result == (prime - 1)) ? -1 : static_cast<int>(result);
}

std::vector<uint8_t> ComputeLegendreSequence(int prime) {
    std::vector<uint8_t> sequence;
    sequence.reserve(static_cast<size_t>(prime));

    for (int t = 0; t < prime; ++t) {
        const int symbol = LegendreSymbol(t, prime);
        sequence.push_back(static_cast<uint8_t>((symbol == 1) ? 1 : 0));
    }

    return sequence;
}

// Thread-safe cache of immutable Legendre sequences.
//
// Get() returns a shared_ptr to an immutable sequence rather than a reference
// into the map. This guarantees the returned data outlives any concurrent
// Clear() call: erasing the map entry only drops the cache's ownership, while
// the caller's shared_ptr keeps the buffer alive for as long as it is needed.
class LegendreCache {
public:
    std::shared_ptr<const std::vector<uint8_t>> Get(int prime) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& entry = cache_[prime];
        if (!entry) {
            entry = std::make_shared<const std::vector<uint8_t>>(
                ComputeLegendreSequence(prime));
        }
        return entry;
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
    }

private:
    std::unordered_map<int, std::shared_ptr<const std::vector<uint8_t>>> cache_;
    std::mutex mutex_;
};

LegendreCache g_legendre_cache;

}  // namespace

std::vector<uint8_t> GenerateLegendreSequence(int prime) {
    // The Legendre symbol is only well-defined for a prime modulus, and a
    // non-positive value would drive an oversized reserve or a divide-by-zero in
    // the symbol computation. Reject anything that is not a valid prime.
    if (!IsPrime(prime)) {
        return {};
    }
    return *g_legendre_cache.Get(prime);
}

void ClearLegendreCache() {
    g_legendre_cache.Clear();
}

std::vector<uint8_t> GenerateWeilPrimary(int prn,
                                         const SpreadingSpecTables& tables,
                                         std::string* error_message) {
    if (prn < 1 || prn > kMaxPrns) {
        if (error_message != nullptr) {
            *error_message = "PRN must be in range [1, 210]";
        }
        return {};
    }

    if (tables.weil_primary_k.size() != static_cast<size_t>(kMaxPrns) ||
        tables.weil_primary_insert_index.size() != static_cast<size_t>(kMaxPrns)) {
        if (error_message != nullptr) {
            *error_message = "Weil primary tables not loaded";
        }
        return {};
    }

    const int k = tables.weil_primary_k[prn - 1];
    const int insert_index = tables.weil_primary_insert_index[prn - 1];
    if (k < 0 || k >= kWeilPrimaryPrime || insert_index < 0 || insert_index >= kWeilPrimaryPrime) {
        if (error_message != nullptr) {
            *error_message = "Invalid Weil primary parameters for PRN " + std::to_string(prn);
        }
        return {};
    }

    const std::shared_ptr<const std::vector<uint8_t>> legendre_holder =
        g_legendre_cache.Get(kWeilPrimaryPrime);
    const std::vector<uint8_t>& legendre = *legendre_holder;
    static constexpr std::array<uint8_t, kExpansionLength> kExpansion = {0, 1, 1, 0, 1, 0, 0};

    std::vector<uint8_t> code;
    code.reserve(static_cast<size_t>(kWeilPrimaryPrime + kExpansionLength));

    if (insert_index == 1) {
        code.insert(code.end(), kExpansion.begin(), kExpansion.end());
    }

    for (int t = 0; t < kWeilPrimaryPrime; ++t) {
        const uint8_t chip = static_cast<uint8_t>(legendre[t] ^ legendre[(t + k) % kWeilPrimaryPrime]);
        code.push_back(chip);

        // insert_index is 1-based, meaning the expansion sequence starts at the insert_index-th chip of the final sequence.
        // Therefore, it is inserted after (insert_index - 1) chips of W have been generated.
        // Since t is 0-based, we insert after t = insert_index - 2.
        if (t == insert_index - 2) {
            code.insert(code.end(), kExpansion.begin(), kExpansion.end());
        }
    }

    return code;
}

std::vector<uint8_t> GenerateWeilTertiary(int prn,
                                          const SpreadingSpecTables& tables,
                                          std::string* error_message) {
    if (prn < 1 || prn > kMaxPrns) {
        if (error_message != nullptr) {
            *error_message = "PRN must be in range [1, 210]";
        }
        return {};
    }

    if (tables.weil_tertiary_k.size() != static_cast<size_t>(kMaxPrns)) {
        if (error_message != nullptr) {
            *error_message = "Weil tertiary table not loaded";
        }
        return {};
    }

    const int k = tables.weil_tertiary_k[prn - 1];
    if (k < 0 || k >= kWeilTertiaryPrime) {
        if (error_message != nullptr) {
            *error_message = "Invalid Weil tertiary parameter for PRN " + std::to_string(prn);
        }
        return {};
    }

    const std::shared_ptr<const std::vector<uint8_t>> legendre_holder =
        g_legendre_cache.Get(kWeilTertiaryPrime);
    const std::vector<uint8_t>& legendre = *legendre_holder;
    std::vector<uint8_t> code;
    code.reserve(kWeilTertiaryLength);

    for (int t = 0; t < kWeilTertiaryPrime; ++t) {
        code.push_back(static_cast<uint8_t>(legendre[t] ^ legendre[(t + k) % kWeilTertiaryPrime]));
    }

    code.push_back(0U);  // Spec-appended terminal zero.
    return code;
}

}  // namespace lunanet::gateway1
