#include "ofcl/continuum/sph_solver.hpp"
#include <cmath>
#include <algorithm>

namespace ofcl {

SphSolver::SphSolver(double smoothing_length) : h_(smoothing_length) {}

void SphSolver::set_particles(std::vector<Particle> particles) {
    particles_ = std::move(particles);
}

double SphSolver::kernel(double r) const noexcept {
    // Cubic spline (1-D normalized roughly)
    double q = r / h_;
    if (q < 1.0) return (1.0 - 1.5 * q * q + 0.75 * q * q * q) / h_;
    if (q < 2.0) {
        double t = 2.0 - q;
        return 0.25 * t * t * t / h_;
    }
    return 0.0;
}

double SphSolver::kernel_grad(double r) const noexcept {
    double q = r / h_;
    if (q < 1.0) return (-3.0 * q + 2.25 * q * q) / (h_ * h_);
    if (q < 2.0) {
        double t = 2.0 - q;
        return -0.75 * t * t / (h_ * h_);
    }
    return 0.0;
}

void SphSolver::step(double dt) {
    // Simplified density / pressure / advection step (research prototype)
    const std::size_t n = particles_.size();
    if (n == 0) return;

    // Density estimation
    for (std::size_t i = 0; i < n; ++i) {
        double rho = 0.0;
        for (std::size_t j = 0; j < n; ++j) {
            double r = std::abs(particles_[i].x - particles_[j].x);
            rho += particles_[j].mass * kernel(r);
        }
        particles_[i].density = std::max(rho, 1e-8);
        particles_[i].pressure = particles_[i].density - 1.0;  // EOS placeholder
    }

    // Acceleration (pressure gradient)
    std::vector<double> ax(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            if (i == j) continue;
            double dx = particles_[i].x - particles_[j].x;
            double r = std::abs(dx);
            if (r < 1e-12) continue;
            double grad = kernel_grad(r) * (dx > 0 ? 1.0 : -1.0);
            double term = particles_[j].mass *
                          (particles_[i].pressure / (particles_[i].density * particles_[i].density) +
                           particles_[j].pressure / (particles_[j].density * particles_[j].density)) *
                          grad;
            ax[i] -= term;
        }
    }

    // Integrate
    for (std::size_t i = 0; i < n; ++i) {
        particles_[i].vx += ax[i] * dt;
        particles_[i].x += particles_[i].vx * dt;
    }
}

void SphSolver::project_to_field(ContinuumField& field) const {
    auto& pts = field.points();
    for (auto& p : pts) {
        p.depth = 0.0;
        p.intensity = 0.0;
        for (const auto& part : particles_) {
            double r = std::abs(p.x - part.x);
            double w = kernel(r);
            p.depth += part.mass * w;
            p.intensity += part.pressure * w;
        }
    }
    field.update_stress();
}

}  // namespace ofcl
