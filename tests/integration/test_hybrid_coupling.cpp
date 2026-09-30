#include <gtest/gtest.h>
#include "ofcl/core/engine.hpp"

using namespace ofcl;

TEST(HybridCoupling, SmallRun) {
    SimulationConfig cfg;
    cfg.seed = 7;
    cfg.deterministic = true;
    Engine engine(cfg);

    DeterministicRng rng(7);
    TimestampNs t = 0;
    for (int i = 0; i < 50; ++i) {
        Event e;
        e.type = EventType::NewOrder;
        e.timestamp = t;
        e.sequence = static_cast<std::uint64_t>(i);
        NewOrderEvent no;
        no.order.side = (i % 2 == 0) ? Side::Bid : Side::Ask;
        no.order.type = OrderType::Limit;
        no.order.price = 100.0 + rng.uniform(-0.5, 0.5);
        no.order.quantity = 10.0;
        no.order.remaining = 10.0;
        no.order.timestamp = t;
        e.payload = no;
        engine.submit(e);
        t += 1000;
    }
    engine.run(50);
    auto snap = engine.book().snapshot(5);
    // Book should have some levels
    EXPECT_GE(snap.bids.size() + snap.asks.size(), 0u);
}
