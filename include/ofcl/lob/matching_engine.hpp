#pragma once

#include "ofcl/types.hpp"
#include "ofcl/core/event.hpp"
#include "ofcl/lob/order_book.hpp"
#include <vector>
#include <functional>

namespace ofcl {

/// Matching engine that consumes events and drives the OrderBook.
class MatchingEngine {
public:
    explicit MatchingEngine(OrderBook& book);

    /// Process a single event; returns generated trades.
    std::vector<Trade> process(const Event& e);

    using TradeCallback = std::function<void(const Trade&)>;
    void set_trade_callback(TradeCallback cb) { trade_cb_ = std::move(cb); }

private:
    OrderBook& book_;
    TradeCallback trade_cb_;
    OrderId next_id_{1};
};

}  // namespace ofcl
