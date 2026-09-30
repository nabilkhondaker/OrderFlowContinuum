#pragma once

#include "ofcl/continuum/mesh.hpp"
#include "ofcl/continuum/field.hpp"

namespace ofcl {

/// Experimental 1-D finite-element solver (piecewise-linear elements).
/// Provided for comparison with SPH; not the primary path.
class FemSolver {
public:
    FemSolver() = default;

    void assemble(const Mesh& mesh, const ContinuumField& field);
    void solve();
    void update_field(ContinuumField& field) const;

private:
    std::vector<double> solution_;
    std::vector<double> rhs_;
};

}  // namespace ofcl
