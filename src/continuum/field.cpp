#include "ofcl/continuum/field.hpp"
#include <cmath>
#include <algorithm>

namespace ofcl {

ContinuumField::ContinuumField(std::size_t n_points, Price x_min, Price x_max)
    : x_min_(x_min), x_max_(x_max) {
    resize(n_points, x_min, x_max);
}

void ContinuumField::resize(std::size_t n, Price x_min, Price x_max) {
    x_min_ = x_min;
    x_max_ = x_max;
    points_.resize(n);
    if (n == 0) return;
    double dx = (x_max - x_min) / static_cast<double>(n - 1);
    for (std::size_t i = 0; i < n; ++i) {
        points_[i].x = x_min + dx * static_cast<double>(i);
        points_[i].depth = 0.0;
        points_[i].intensity = 0.0;
        points_[i].imbalance = 0.0;
        points_[i].stress = {};
    }
}

void ContinuumField::clear() {
    for (auto& p : points_) {
        p.depth = p.intensity = p.imbalance = 0.0;
        p.stress = {};
    }
}

void ContinuumField::project_from_book(
    const std::vector<std::pair<Price, Quantity>>& bids,
    const std::vector<std::pair<Price, Quantity>>& asks,
    double intensity_scale) {
    clear();
    if (points_.empty()) return;

    auto accumulate = [&](const std::vector<std::pair<Price, Quantity>>& levels,
                          double sign) {
        for (const auto& [px, qty] : levels) {
            // Nearest-node injection (simple; higher-order kernels possible)
            std::size_t best = 0;
            double best_d = std::abs(points_[0].x - px);
            for (std::size_t i = 1; i < points_.size(); ++i) {
                double d = std::abs(points_[i].x - px);
                if (d < best_d) { best_d = d; best = i; }
            }
            points_[best].depth += qty;
            points_[best].imbalance += sign * qty;
        }
    };
    accumulate(bids, +1.0);
    accumulate(asks, -1.0);

    for (auto& p : points_) {
        double tot = std::abs(p.imbalance) + 1e-12;
        p.imbalance = p.imbalance / (p.depth + 1e-12);
        p.intensity = intensity_scale * p.depth;  // placeholder
    }
}

void ContinuumField::update_stress(double alpha, double beta, double gamma) {
    for (std::size_t i = 0; i < points_.size(); ++i) {
        double grad = 0.0;
        if (i > 0 && i + 1 < points_.size()) {
            grad = (points_[i + 1].depth - points_[i - 1].depth) /
                   (points_[i + 1].x - points_[i - 1].x + 1e-12);
        }
        double p = alpha * points_[i].intensity + beta * points_[i].depth +
                   gamma * std::abs(grad);
        double shear = points_[i].imbalance * p;
        points_[i].stress = make_stress(p, shear);
    }
}

}  // namespace ofcl
