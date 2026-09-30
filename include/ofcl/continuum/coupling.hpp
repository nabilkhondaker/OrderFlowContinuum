#pragma once

#include "ofcl/lob/order_book.hpp"
#include "ofcl/continuum/field.hpp"
#include "ofcl/continuum/sph_solver.hpp"
#include "ofcl/continuum/adaptive_mesh.hpp"
#include "ofcl/continuum/mesh.hpp"
#include <memory>
#include <string>

namespace ofcl {

/// Bridge between discrete LOB events / snapshots and the continuum layer.
class ContinuumCoupling {
public:
    explicit ContinuumCoupling(const std::string& solver = "sph",
                               bool adaptive = true,
                               double refine_threshold = 0.05);

    /// Update continuum state from a book snapshot and recent order-flow stats.
    void update_from_snapshot(const OrderBook::Snapshot& snap,
                              double recent_intensity = 0.0);

    /// Advance continuum time by dt (SPH or FEM step).
    void step(double dt);

    const ContinuumField& field() const noexcept { return field_; }
    const Mesh& mesh() const noexcept { return mesh_; }
    const SphSolver* sph() const noexcept { return sph_.get(); }

    double residual_norm() const noexcept { return residual_; }

private:
    ContinuumField field_;
    Mesh mesh_;
    AdaptiveMesh adaptive_;
    std::unique_ptr<SphSolver> sph_;
    std::string solver_type_;
    double residual_{0.0};
};

}  // namespace ofcl
