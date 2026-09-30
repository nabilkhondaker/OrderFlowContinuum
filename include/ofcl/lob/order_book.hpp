#pragma once

#include "ofcl/types.hpp"
#include "ofcl/lob/price_level.hpp"
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <mutex>

namespace ofcl {

/// Classic price-time priority limit order book.
/// Concurrent readers may use a snapshot; writers are expected to be
/// serialized by the matching engine or protected by the internal mutex
/// when multi-threaded access is enabled.
class OrderBook {
public:
    explicit OrderBook(double tick_size = 0.01, int max_depth = 100);

    // Order management
    bool add(const Order& order);
    bool cancel(OrderId id);
    bool amend(OrderId id, Price new_price, Quantity new_qty);

    // Matching helpers
    std::vector<Trade> match_market(Side side, Quantity qty, OrderId aggressor,
                                    TimestampNs ts);
    std::vector<Trade> match_limit(const Order& order);

    // Queries
    std::optional<Price> best_bid() const;
    std::optional<Price> best_ask() const;
    Quantity depth_at(Side side, Price px) const;
    std::vector<std::pair<Price, Quantity>> levels(Side side, int depth) const;

    // Snapshot for continuum coupling / visualization
    struct Snapshot {
        TimestampNs timestamp{0};
        std::vector<std::pair<Price, Quantity>> bids;
        std::vector<std::pair<Price, Quantity>> asks;
        Price mid{0.0};
        double imbalance{0.0};
    };
    Snapshot snapshot(int depth = 20) const;

    std::size_t order_count() const;
    void clear();

private:
    double tick_size_;
    int max_depth_;
    // Higher price first for bids, lower first for asks
    std::map<Price, PriceLevel, std::greater<Price>> bids_;
    std::map<Price, PriceLevel, std::less<Price>> asks_;
    std::unordered_map<OrderId, std::pair<Side, Price>> order_index_;
    mutable std::mutex mu_;
};

}  // namespace ofcl
