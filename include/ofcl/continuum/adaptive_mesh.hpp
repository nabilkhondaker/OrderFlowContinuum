#pragma once

#include "ofcl/continuum/mesh.hpp"
#include "ofcl/continuum/field.hpp"

namespace ofcl {

/// Adaptive mesh refinement driven by continuum field gradients / stress.
class AdaptiveMesh {
public:
    AdaptiveMesh(double refine_threshold = 0.05, double coarsen_threshold = 0.01);

    void refine(Mesh& mesh, const ContinuumField& field);
    double last_max_indicator() const noexcept { return last_max_; }

private:
    double refine_threshold_;
    double coarsen_threshold_;
    double last_max_{0.0};
};

}  // namespace ofcl
