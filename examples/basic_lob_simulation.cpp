#include "ofcl/lob/order_book.hpp"
#include "ofcl/lob/matching_engine.hpp"
#include "ofcl/core/deterministic_rng.hpp"
#include <iostream>

int main() {
    ofcl::OrderBook book(0.01, 20);
    ofcl::MatchingEngine me(book);
    ofcl::DeterministicRng rng(42);

    for (int i = 0; i < 20; ++i) {
        ofcl::Event e;
        e.type = ofcl::EventType::NewOrder;
        e.timestamp = i * 1000;
        ofcl::NewOrderEvent no;
        no.order.side = (i % 2 == 0) ? ofcl::Side::Bid : ofcl::Side::Ask;
        no.order.type = ofcl::OrderType::Limit;
        no.order.price = 100.0 + rng.uniform(-0.5, 0.5);
        no.order.quantity = 10.0;
        no.order.remaining = 10.0;
        e.payload = no;
        me.process(e);
    }
    auto snap = book.snapshot(5);
    std::cout << "Bids: " << snap.bids.size() << "  Asks: " << snap.asks.size() << "\n";
    return 0;
}
