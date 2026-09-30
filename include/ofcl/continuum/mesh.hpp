#pragma once

#include "ofcl/types.hpp"
#include <vector>

namespace ofcl {

struct MeshNode {
    Price x{0.0};
    std::size_t level{0};  // refinement level
};

/// Simple 1-D adaptive mesh on the price axis.
class Mesh {
public:
    explicit Mesh(std::size_t n = 32, Price x_min = 0.0, Price x_max = 100.0);

    const std::vector<MeshNode>& nodes() const noexcept { return nodes_; }
    std::size_t size() const noexcept { return nodes_.size(); }

    void uniform_refine();
    void adapt(const std::vector<double>& indicator, double threshold);

private:
    std::vector<MeshNode> nodes_;
    Price x_min_{0.0};
    Price x_max_{100.0};
};

}  // namespace ofcl
