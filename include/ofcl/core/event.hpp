#pragma once

#include "ofcl/types.hpp"
#include <variant>
#include <optional>

namespace ofcl {

struct NewOrderEvent {
    Order order;
};

struct CancelEvent {
    OrderId order_id;
    TimestampNs timestamp;
};

struct AmendEvent {
    OrderId order_id;
    Price new_price;
    Quantity new_quantity;
    TimestampNs timestamp;
};

struct TradeEvent {
    Trade trade;
};

using EventPayload = std::variant<NewOrderEvent, CancelEvent, AmendEvent, TradeEvent>;

struct Event {
    EventType type{EventType::NewOrder};
    TimestampNs timestamp{0};
    EventPayload payload;
    std::uint64_t sequence{0};  // for deterministic ordering
};

}  // namespace ofcl
