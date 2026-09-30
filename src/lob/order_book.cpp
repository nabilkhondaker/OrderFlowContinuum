#include "ofcl/lob/order_book.hpp"
#include <algorithm>
#include <cmath>

namespace ofcl {

OrderBook::OrderBook(double tick_size, int max_depth)
    : tick_size_(tick_size), max_depth_(max_depth) {}

bool OrderBook::add(const Order& order) {
    std::lock_guard lock(mu_);
    if (order_index_.count(order.id)) return false;

    if (order.side == Side::Bid) {
        auto& lvl = bids_[order.price];
        lvl.price = order.price;
        lvl.add(order);
    } else {
        auto& lvl = asks_[order.price];
        lvl.price = order.price;
        lvl.add(order);
    }
    order_index_[order.id] = {order.side, order.price};
    return true;
}

bool OrderBook::cancel(OrderId id) {
    std::lock_guard lock(mu_);
    auto it = order_index_.find(id);
    if (it == order_index_.end()) return false;

    Side side = it->second.first;
    Price px = it->second.second;
    order_index_.erase(it);

    auto remove_from = [&](auto& book) {
        auto lit = book.find(px);
        if (lit == book.end()) return;
        auto& lvl = lit->second;
        for (auto oit = lvl.orders.begin(); oit != lvl.orders.end(); ++oit) {
            if (oit->id == id) {
                lvl.total_quantity -= oit->remaining;
                lvl.orders.erase(oit);
                break;
            }
        }
        if (lvl.orders.empty()) book.erase(lit);
    };

    if (side == Side::Bid) remove_from(bids_);
    else remove_from(asks_);
    return true;
}

bool OrderBook::amend(OrderId id, Price new_price, Quantity new_qty) {
    // Simplified: cancel + re-add
    std::lock_guard lock(mu_);
    auto it = order_index_.find(id);
    if (it == order_index_.end()) return false;
    Side side = it->second.first;
    // Release lock for cancel/add would be racy; do inline for now
    // (full implementation would be more careful)
    (void)side;
    (void)new_price;
    (void)new_qty;
    return false;  // stub: amend not fully implemented in skeleton
}

std::vector<Trade> OrderBook::match_market(Side side, Quantity qty,
                                           OrderId aggressor, TimestampNs ts) {
    std::lock_guard lock(mu_);
    std::vector<Trade> trades;
    if (side == Side::Bid) {
        // Buy: lift asks
        while (qty > 0.0 && !asks_.empty()) {
            auto it = asks_.begin();
            Quantity filled = it->second.match(qty, trades, aggressor, ts, side);
            qty -= filled;
            if (it->second.orders.empty()) asks_.erase(it);
            else break;
        }
    } else {
        // Sell: hit bids
        while (qty > 0.0 && !bids_.empty()) {
            auto it = bids_.begin();
            Quantity filled = it->second.match(qty, trades, aggressor, ts, side);
            qty -= filled;
            if (it->second.orders.empty()) bids_.erase(it);
            else break;
        }
    }
    return trades;
}

std::vector<Trade> OrderBook::match_limit(const Order& order) {
    std::vector<Trade> trades;
    if (order.type == OrderType::Market || order.type == OrderType::IOC ||
        order.type == OrderType::FOK) {
        return match_market(order.side, order.quantity, order.id, order.timestamp);
    }

    // Limit: first try to match against opposite side
    std::lock_guard lock(mu_);
    Quantity remaining = order.quantity;
    if (order.side == Side::Bid) {
        while (remaining > 0.0 && !asks_.empty()) {
            auto it = asks_.begin();
            if (it->first > order.price) break;  // not marketable
            Quantity filled = it->second.match(remaining, trades, order.id,
                                               order.timestamp, order.side);
            remaining -= filled;
            if (it->second.orders.empty()) asks_.erase(it);
            else break;
        }
    } else {
        while (remaining > 0.0 && !bids_.empty()) {
            auto it = bids_.begin();
            if (it->first < order.price) break;
            Quantity filled = it->second.match(remaining, trades, order.id,
                                               order.timestamp, order.side);
            remaining -= filled;
            if (it->second.orders.empty()) bids_.erase(it);
            else break;
        }
    }

    // Rest remaining as limit if any
    if (remaining > 0.0 && order.type == OrderType::Limit) {
        Order resting = order;
        resting.remaining = remaining;
        if (order.side == Side::Bid) {
            auto& lvl = bids_[order.price];
            lvl.price = order.price;
            lvl.add(resting);
        } else {
            auto& lvl = asks_[order.price];
            lvl.price = order.price;
            lvl.add(resting);
        }
        order_index_[order.id] = {order.side, order.price};
    }
    return trades;
}

std::optional<Price> OrderBook::best_bid() const {
    std::lock_guard lock(mu_);
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<Price> OrderBook::best_ask() const {
    std::lock_guard lock(mu_);
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

Quantity OrderBook::depth_at(Side side, Price px) const {
    std::lock_guard lock(mu_);
    if (side == Side::Bid) {
        auto it = bids_.find(px);
        return it == bids_.end() ? 0.0 : it->second.total_quantity;
    }
    auto it = asks_.find(px);
    return it == asks_.end() ? 0.0 : it->second.total_quantity;
}

std::vector<std::pair<Price, Quantity>> OrderBook::levels(Side side, int depth) const {
    std::lock_guard lock(mu_);
    std::vector<std::pair<Price, Quantity>> out;
    if (side == Side::Bid) {
        for (const auto& [px, lvl] : bids_) {
            out.emplace_back(px, lvl.total_quantity);
            if (static_cast<int>(out.size()) >= depth) break;
        }
    } else {
        for (const auto& [px, lvl] : asks_) {
            out.emplace_back(px, lvl.total_quantity);
            if (static_cast<int>(out.size()) >= depth) break;
        }
    }
    return out;
}

OrderBook::Snapshot OrderBook::snapshot(int depth) const {
    std::lock_guard lock(mu_);
    Snapshot s;
    for (const auto& [px, lvl] : bids_) {
        s.bids.emplace_back(px, lvl.total_quantity);
        if (static_cast<int>(s.bids.size()) >= depth) break;
    }
    for (const auto& [px, lvl] : asks_) {
        s.asks.emplace_back(px, lvl.total_quantity);
        if (static_cast<int>(s.asks.size()) >= depth) break;
    }
    if (!s.bids.empty() && !s.asks.empty()) {
        s.mid = 0.5 * (s.bids.front().first + s.asks.front().first);
        double bid_vol = 0.0, ask_vol = 0.0;
        for (const auto& p : s.bids) bid_vol += p.second;
        for (const auto& p : s.asks) ask_vol += p.second;
        double tot = bid_vol + ask_vol;
        s.imbalance = tot > 0.0 ? (bid_vol - ask_vol) / tot : 0.0;
    }
    return s;
}

std::size_t OrderBook::order_count() const {
    std::lock_guard lock(mu_);
    return order_index_.size();
}

void OrderBook::clear() {
    std::lock_guard lock(mu_);
    bids_.clear();
    asks_.clear();
    order_index_.clear();
}

}  // namespace ofcl
