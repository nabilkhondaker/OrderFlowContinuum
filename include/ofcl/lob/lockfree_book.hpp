#pragma once

#include "ofcl/types.hpp"
#include <atomic>
#include <array>
#include <cstdint>

namespace ofcl {

/// Experimental lock-free price-level ring for high-contention scenarios.
/// This is a research component; the primary OrderBook remains the
/// authoritative matching structure. The lock-free variant is used for
/// latency experiments and concurrent snapshot publishing.
class LockFreeBookSide {
public:
    static constexpr std::size_t kMaxLevels = 256;

    struct Level {
        std::atomic<double> price{0.0};
        std::atomic<double> quantity{0.0};
        std::atomic<std::uint64_t> version{0};
    };

    LockFreeBookSide() = default;

    void publish_level(std::size_t idx, Price px, Quantity qty) noexcept;
    bool read_level(std::size_t idx, Price& px, Quantity& qty) const noexcept;

private:
    std::array<Level, kMaxLevels> levels_{};
};

}  // namespace ofcl
