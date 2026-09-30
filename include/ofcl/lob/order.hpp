#pragma once

#include "ofcl/types.hpp"

namespace ofcl {

// Order is defined in types.hpp; this header provides additional helpers.

inline bool is_aggressive(OrderType t) noexcept {
    return t == OrderType::Market || t == OrderType::IOC || t == OrderType::FOK;
}

inline const char* side_str(Side s) noexcept {
    return s == Side::Bid ? "Bid" : "Ask";
}

inline const char* order_type_str(OrderType t) noexcept {
    switch (t) {
        case OrderType::Limit:  return "Limit";
        case OrderType::Market: return "Market";
        case OrderType::Cancel: return "Cancel";
        case OrderType::Amend:  return "Amend";
        case OrderType::IOC:    return "IOC";
        case OrderType::FOK:    return "FOK";
        default:                return "Unknown";
    }
}

}  // namespace ofcl
