#pragma once

#include "ofcl/types.hpp"
#include "ofcl/core/deterministic_rng.hpp"
#include <string>

namespace ofcl {

class LatencyModel {
public:
    LatencyModel(TimestampNs me_latency_ns = 1200,
                  const std::string& network_model = "lognormal",
                  bool colocation = false);

    /// Sample total latency for a participant (network + ME + queue).
    TimestampNs sample(DeterministicRng& rng, bool is_colocated = false) const;

    TimestampNs matching_engine_latency() const noexcept { return me_latency_; }

private:
    TimestampNs me_latency_;
    std::string network_model_;
    bool colocation_default_;
};

}  // namespace ofcl
