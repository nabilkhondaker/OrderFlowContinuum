#include "ofcl/latency/latency_model.hpp"
#include <algorithm>

namespace ofcl {

LatencyModel::LatencyModel(TimestampNs me_latency_ns,
                           const std::string& network_model,
                           bool colocation)
    : me_latency_(me_latency_ns),
      network_model_(network_model),
      colocation_default_(colocation) {}

TimestampNs LatencyModel::sample(DeterministicRng& rng, bool is_colocated) const {
    TimestampNs network = 0;
    if (network_model_ == "lognormal") {
        // mean ~ 5 us, with heavy tail
        double us = rng.lognormal(1.5, 0.6);  // ~ microseconds
        network = static_cast<TimestampNs>(us * 1000.0);  // to ns
    } else {
        network = static_cast<TimestampNs>(rng.uniform(1000.0, 50000.0));
    }
    if (is_colocated || colocation_default_) {
        network = static_cast<TimestampNs>(network * 0.1);  // co-lo advantage
    }
    TimestampNs queue = static_cast<TimestampNs>(rng.uniform(0.0, 500.0));
    return me_latency_ + network + queue;
}

}  // namespace ofcl
