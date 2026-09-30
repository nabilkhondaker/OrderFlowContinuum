#pragma once

#include "ofcl/continuum/field.hpp"
#include <vector>

namespace ofcl {

/// Simplified SPH-style solver on the 1-D liquidity surface.
/// Particles carry depth / intensity; kernel interpolation produces continuum fields.
class SphSolver {
public:
    struct Particle {
        Price x{0.0};
        double mass{1.0};
        double density{1.0};
        double pressure{0.0};
        double vx{0.0};  // macroscopic flow velocity
    };

    explicit SphSolver(double smoothing_length = 0.5);

    void set_particles(std::vector<Particle> particles);
    const std::vector<Particle>& particles() const noexcept { return particles_; }

    /// One SPH step: density estimation, pressure, artificial viscosity, advection.
    void step(double dt);

    /// Project particle state onto a ContinuumField for visualization / coupling.
    void project_to_field(ContinuumField& field) const;

private:
    double h_;  // smoothing length
    std::vector<Particle> particles_;
    double kernel(double r) const noexcept;
    double kernel_grad(double r) const noexcept;
};

}  // namespace ofcl
