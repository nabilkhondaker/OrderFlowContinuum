#pragma once

#include "ofcl/types.hpp"
#include <deque>
#include <atomic>

namespace ofcl {

/// Single price level holding a FIFO queue of resting orders.
/// Designed for cache-friendly access; the queue itself is not lock-free
/// (locking or single-threaded access is expected for matching).
struct PriceLevel {
    Price price{0.0};
    Quantity total_quantity{0.0};
    std::deque<Order> orders;  // price-time priority FIFO

    void add(const Order& o) {
        orders.push_back(o);
        total_quantity += o.remaining;
    }

    /// Remove front quantity; returns filled amount and whether level emptied.
    Quantity match(Quantity qty, std::vector<Trade>& trades, OrderId aggressor,
                   TimestampNs ts, Side aggressor_side);
};

}  // namespace ofcl
