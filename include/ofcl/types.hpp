#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <array>
#include <optional>

namespace ofcl {

// Nanosecond-resolution timestamp (monotonic simulation time)
using TimestampNs = std::int64_t;
using Price = double;
using Quantity = double;
using OrderId = std::uint64_t;
using LevelId = std::uint32_t;

enum class Side : std::uint8_t {
    Bid = 0,
    Ask = 1
};

enum class OrderType : std::uint8_t {
    Limit = 0,
    Market = 1,
    Cancel = 2,
    Amend = 3,
    IOC = 4,   // Immediate-or-Cancel
    FOK = 5    // Fill-or-Kill
};

enum class EventType : std::uint8_t {
    NewOrder = 0,
    Cancel = 1,
    Amend = 2,
    Trade = 3,
    Snapshot = 4
};

struct Order {
    OrderId id{0};
    Side side{Side::Bid};
    OrderType type{OrderType::Limit};
    Price price{0.0};
    Quantity quantity{0.0};
    Quantity remaining{0.0};
    TimestampNs timestamp{0};
    std::uint32_t participant_id{0};
};

struct Trade {
    OrderId aggressor_id{0};
    OrderId resting_id{0};
    Price price{0.0};
    Quantity quantity{0.0};
    TimestampNs timestamp{0};
    Side aggressor_side{Side::Bid};
};

// Continuum field quantities
struct StressTensor {
    // 1-D liquidity surface: simplified representation
    double sigma_xx{0.0};  // normal stress along price axis
    double sigma_xy{0.0};  // shear component (imbalance-driven)
    double pressure{0.0};

    double norm() const noexcept {
        return std::sqrt(sigma_xx * sigma_xx + sigma_xy * sigma_xy + pressure * pressure);
    }
};

struct FieldPoint {
    Price x{0.0};           // price coordinate
    double depth{0.0};      // standing liquidity
    double intensity{0.0};  // aggressive flow intensity
    double imbalance{0.0};  // (bid - ask) / (bid + ask)
    StressTensor stress{};
};

// Configuration snapshot for reproducibility
struct RunMetadata {
    std::string config_hash;
    std::uint64_t seed{0};
    std::string version;
    std::string compiler;
    TimestampNs start_time{0};
};

}  // namespace ofcl
