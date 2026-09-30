#pragma once

#include <cstdint>
#include <random>
#include <limits>

namespace ofcl {

/// Deterministic pseudo-random number generator for reproducible simulations.
/// Uses a fixed seed and a well-defined algorithm so that identical seeds
/// produce identical sequences across platforms (within IEEE floating-point
/// limits).
class DeterministicRng {
public:
    explicit DeterministicRng(std::uint64_t seed = 42) noexcept;

    void seed(std::uint64_t s) noexcept;
    std::uint64_t current_seed() const noexcept { return seed_; }

    /// Uniform [0, 1)
    double uniform01() noexcept;

    /// Uniform [lo, hi)
    double uniform(double lo, double hi) noexcept;

    /// Normal (Box-Muller, deterministic)
    double normal(double mean = 0.0, double stddev = 1.0) noexcept;

    /// Log-normal
    double lognormal(double mean, double stddev) noexcept;

    std::uint64_t uniform_uint64() noexcept;

private:
    std::uint64_t seed_{42};
    std::mt19937_64 eng_;
    bool has_spare_{false};
    double spare_{0.0};
};

}  // namespace ofcl
