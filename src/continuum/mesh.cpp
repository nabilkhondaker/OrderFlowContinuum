#include "ofcl/continuum/mesh.hpp"
#include <algorithm>

namespace ofcl {

Mesh::Mesh(std::size_t n, Price x_min, Price x_max)
    : x_min_(x_min), x_max_(x_max) {
    nodes_.resize(n);
    if (n == 0) return;
    double dx = (x_max - x_min) / static_cast<double>(n - 1);
    for (std::size_t i = 0; i < n; ++i) {
        nodes_[i].x = x_min + dx * static_cast<double>(i);
        nodes_[i].level = 0;
    }
}

void Mesh::uniform_refine() {
    if (nodes_.size() < 2) return;
    std::vector<MeshNode> refined;
    refined.reserve(nodes_.size() * 2 - 1);
    for (std::size_t i = 0; i + 1 < nodes_.size(); ++i) {
        refined.push_back(nodes_[i]);
        MeshNode mid;
        mid.x = 0.5 * (nodes_[i].x + nodes_[i + 1].x);
        mid.level = std::max(nodes_[i].level, nodes_[i + 1].level) + 1;
        refined.push_back(mid);
    }
    refined.push_back(nodes_.back());
    nodes_ = std::move(refined);
}

void Mesh::adapt(const std::vector<double>& indicator, double threshold) {
    if (indicator.size() != nodes_.size()) return;
    // Simple: mark nodes with high indicator for local refinement
    // (full hanging-node / coarsening logic left for future work)
    bool any = false;
    for (double v : indicator) if (v > threshold) { any = true; break; }
    if (any) uniform_refine();
}

}  // namespace ofcl
