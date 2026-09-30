#pragma once

#include "ofcl/types.hpp"
#include "ofcl/core/timestamp.hpp"

namespace ofcl {

/// Monotonic simulation clock with nanosecond resolution.
/// Advances only by explicit calls; never reads wall time for simulation logic.
class SimulationClock {
public:
    explicit SimulationClock(TimestampNs start = 0) noexcept : now_(start) {}

    TimestampNs now() const noexcept { return now_; }
    Timestamp timestamp() const noexcept { return Timestamp(now_); }

    void advance(TimestampNs delta) noexcept { now_ += delta; }
    void set(TimestampNs t) noexcept { now_ = t; }

    /// Advance to at least the given time (idempotent if already later).
    void advance_to(TimestampNs t) noexcept {
        if (t > now_) now_ = t;
    }

private:
    TimestampNs now_{0};
};

}  // namespace ofcl
