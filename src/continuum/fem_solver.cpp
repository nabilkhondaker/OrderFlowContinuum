#include "ofcl/continuum/fem_solver.hpp"

namespace ofcl {

void FemSolver::assemble(const Mesh& mesh, const ContinuumField& field) {
    (void)mesh;
    (void)field;
    // Placeholder: full FEM assembly not implemented in skeleton
    solution_.assign(mesh.size(), 0.0);
    rhs_.assign(mesh.size(), 0.0);
}

void FemSolver::solve() {
    // Placeholder linear solve
}

void FemSolver::update_field(ContinuumField& field) const {
    auto& pts = field.points();
    for (std::size_t i = 0; i < pts.size() && i < solution_.size(); ++i) {
        pts[i].depth = solution_[i];
    }
}

}  // namespace ofcl
