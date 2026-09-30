#include "ofcl/lob/price_level.hpp"
#include <algorithm>

namespace ofcl {

Quantity PriceLevel::match(Quantity qty, std::vector<Trade>& trades,
                           OrderId aggressor, TimestampNs ts,
                           Side aggressor_side) {
    Quantity filled = 0.0;
    while (qty > 0.0 && !orders.empty()) {
        Order& resting = orders.front();
        Quantity take = std::min(qty, resting.remaining);
        Trade t;
        t.aggressor_id = aggressor;
        t.resting_id = resting.id;
        t.price = price;
        t.quantity = take;
        t.timestamp = ts;
        t.aggressor_side = aggressor_side;
        trades.push_back(t);

        resting.remaining -= take;
        total_quantity -= take;
        qty -= take;
        filled += take;

        if (resting.remaining <= 0.0) {
            orders.pop_front();
        }
    }
    return filled;
}

}  // namespace ofcl
