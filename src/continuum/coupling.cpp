#include "ofcl/continuum/coupling.hpp"
#include <cmath>

namespace ofcl {

ContinuumCoupling::ContinuumCoupling(const std::string& solver, bool adaptive,
                                     double refine_threshold)
    : field_(64, 90.0, 110.0),
      mesh_(32, 90.0, 110.0),
      adaptive_(refine_threshold),
      solver_type_(solver) {
    if (solver == "sph") {
        sph_ = std::make_unique<SphSolver>(0.5);
    }
    (void)adaptive;
}

void ContinuumCoupling::update_from_snapshot(const OrderBook::Snapshot& snap,
                                             double recent_intensity) {
    field_.project_from_book(snap.bids, snap.asks, recent_intensity);
    field_.update_stress();

    if (sph_) {
        std::vector<SphSolver::Particle> parts;
        for (const auto& p : field_.points()) {
            SphSolver::Particle part;
            part.x = p.x;
            part.mass = std::max(p.depth, 1e-6);
            part.density = 1.0;
            part.pressure = p.stress.pressure;
            parts.push_back(part);
        }
        sph_->set_particles(std::move(parts));
    }

    // Simple residual: L2 difference between book depth and field depth
    residual_ = 0.0;
    // (full residual computation left for metrics module)
}

void ContinuumCoupling::step(double dt) {
    if (sph_) {
        sph_->step(dt);
        sph_->project_to_field(field_);
    }
    adaptive_.refine(mesh_, field_);
}

}  // namespace ofcl
