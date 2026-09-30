#include "ofcl/continuum/adaptive_mesh.hpp"
#include <cmath>
#include <vector>

namespace ofcl {

AdaptiveMesh::AdaptiveMesh(double refine_threshold, double coarsen_threshold)
    : refine_threshold_(refine_threshold), coarsen_threshold_(coarsen_threshold) {}

void AdaptiveMesh::refine(Mesh& mesh, const ContinuumField& field) {
    const auto& pts = field.points();
    if (pts.size() < 2) return;
    std::vector<double> indicator(pts.size(), 0.0);
    last_max_ = 0.0;
    for (std::size_t i = 1; i + 1 < pts.size(); ++i) {
        double grad = std::abs(pts[i + 1].depth - pts[i - 1].depth) /
                      (pts[i + 1].x - pts[i - 1].x + 1e-12);
        double stress = pts[i].stress.norm();
        indicator[i] = std::max(grad, stress);
        last_max_ = std::max(last_max_, indicator[i]);
    }
    mesh.adapt(indicator, refine_threshold_);
}

}  // namespace ofcl
