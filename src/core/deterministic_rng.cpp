#include "ofcl/core/deterministic_rng.hpp"
#include <cmath>

namespace ofcl {

DeterministicRng::DeterministicRng(std::uint64_t seed) noexcept {
    this->seed(seed);
}

void DeterministicRng::seed(std::uint64_t s) noexcept {
    seed_ = s;
    eng_.seed(s);
    has_spare_ = false;
}

double DeterministicRng::uniform01() noexcept {
    return std::generate_canonical<double, 53>(eng_);
}

double DeterministicRng::uniform(double lo, double hi) noexcept {
    return lo + (hi - lo) * uniform01();
}

double DeterministicRng::normal(double mean, double stddev) noexcept {
    if (has_spare_) {
        has_spare_ = false;
        return mean + stddev * spare_;
    }
    double u, v, s;
    do {
        u = uniform01() * 2.0 - 1.0;
        v = uniform01() * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    s = std::sqrt(-2.0 * std::log(s) / s);
    spare_ = v * s;
    has_spare_ = true;
    return mean + stddev * (u * s);
}

double DeterministicRng::lognormal(double mean, double stddev) noexcept {
    return std::exp(normal(mean, stddev));
}

std::uint64_t DeterministicRng::uniform_uint64() noexcept {
    return eng_();
}

}  // namespace ofcl
