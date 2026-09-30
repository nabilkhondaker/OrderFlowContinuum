#pragma once

#include "ofcl/types.hpp"
#include <vector>

namespace ofcl {

/// One-dimensional continuum field defined on the liquidity surface (price axis).
class ContinuumField {
public:
    explicit ContinuumField(std::size_t n_points = 64, Price x_min = 0.0, Price x_max = 100.0);

    std::size_t size() const noexcept { return points_.size(); }
    const std::vector<FieldPoint>& points() const noexcept { return points_; }
    std::vector<FieldPoint>& points() noexcept { return points_; }

    void resize(std::size_t n, Price x_min, Price x_max);
    void clear();

    /// Project a discrete book snapshot onto the field.
    void project_from_book(const std::vector<std::pair<Price, Quantity>>& bids,
                           const std::vector<std::pair<Price, Quantity>>& asks,
                           double intensity_scale = 1.0);

    /// Compute simple stress from local imbalance and depth gradients.
    void update_stress(double alpha = 1.0, double beta = 0.5, double gamma = 0.2);

private:
    std::vector<FieldPoint> points_;
    Price x_min_{0.0};
    Price x_max_{100.0};
};

}  // namespace ofcl
