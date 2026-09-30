#include "ofcl/lob/matching_engine.hpp"

namespace ofcl {

MatchingEngine::MatchingEngine(OrderBook& book) : book_(book) {}

std::vector<Trade> MatchingEngine::process(const Event& e) {
    std::vector<Trade> trades;
    switch (e.type) {
        case EventType::NewOrder: {
            const auto& no = std::get<NewOrderEvent>(e.payload);
            Order o = no.order;
            if (o.id == 0) o.id = next_id_++;
            o.timestamp = e.timestamp;
            trades = book_.match_limit(o);
            break;
        }
        case EventType::Cancel: {
            const auto& c = std::get<CancelEvent>(e.payload);
            book_.cancel(c.order_id);
            break;
        }
        case EventType::Amend: {
            const auto& a = std::get<AmendEvent>(e.payload);
            book_.amend(a.order_id, a.new_price, a.new_quantity);
            break;
        }
        case EventType::Trade:
            // already matched; ignore or record
            break;
        default:
            break;
    }
    if (trade_cb_) {
        for (const auto& t : trades) trade_cb_(t);
    }
    return trades;
}

}  // namespace ofcl
