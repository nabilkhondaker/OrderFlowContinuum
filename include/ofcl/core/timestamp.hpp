#pragma once

#include "ofcl/types.hpp"
#include <chrono>
#include <string>

namespace ofcl {

/// Simulation clock ticks in nanoseconds.
/// Wall-clock conversion helpers are provided for logging only.
class Timestamp {
public:
    explicit Timestamp(TimestampNs ns = 0) noexcept : ns_(ns) {}

    TimestampNs ns() const noexcept { return ns_; }
    double seconds() const noexcept { return static_cast<double>(ns_) * 1e-9; }

    Timestamp operator+(TimestampNs delta) const noexcept { return Timestamp(ns_ + delta); }
    Timestamp& operator+=(TimestampNs delta) noexcept { ns_ += delta; return *this; }

    bool operator<(const Timestamp& o) const noexcept { return ns_ < o.ns_; }
    bool operator<=(const Timestamp& o) const noexcept { return ns_ <= o.ns_; }
    bool operator==(const Timestamp& o) const noexcept { return ns_ == o.ns_; }

    static Timestamp from_seconds(double s) noexcept {
        return Timestamp(static_cast<TimestampNs>(s * 1e9));
    }

    std::string to_string() const;

private:
    TimestampNs ns_{0};
};

}  // namespace ofcl
