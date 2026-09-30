#pragma once

#include "ofcl/types.hpp"

namespace ofcl {

// StressTensor is defined in types.hpp.
// Additional continuum-specific helpers can live here.

inline StressTensor make_stress(double pressure, double shear) noexcept {
    StressTensor s;
    s.pressure = pressure;
    s.sigma_xx = pressure;
    s.sigma_xy = shear;
    return s;
}

}  // namespace ofcl
