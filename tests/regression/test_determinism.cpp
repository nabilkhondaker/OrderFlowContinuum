#include <gtest/gtest.h>
#include "ofcl/core/engine.hpp"

using namespace ofcl;

TEST(Determinism, SameSeedSameBookState) {
    auto run_once = [](std::uint64_t seed) {
        SimulationConfig cfg;
        cfg.seed = seed;
        cfg.deterministic = true;
        Engine engine(cfg);
        DeterministicRng rng(seed);
        TimestampNs t = 0;
        for (int i = 0; i < 20; ++i) {
            Event e;
            e.type = EventType::NewOrder;
            e.timestamp = t;
            NewOrderEvent no;
            no.order.side = (rng.uniform01() < 0.5) ? Side::Bid : Side::Ask;
            no.order.type = OrderType::Limit;
            no.order.price = 100.0 + rng.uniform(-1.0, 1.0);
            no.order.quantity = rng.uniform(1.0, 20.0);
            no.order.remaining = no.order.quantity;
            e.payload = no;
            engine.submit(e);
            t += 500;
        }
        engine.run(20);
        return engine.book().snapshot(10);
    };

    auto s1 = run_once(99);
    auto s2 = run_once(99);
    EXPECT_EQ(s1.bids.size(), s2.bids.size());
    EXPECT_EQ(s1.asks.size(), s2.asks.size());
    if (!s1.bids.empty() && !s2.bids.empty()) {
        EXPECT_DOUBLE_EQ(s1.bids[0].first, s2.bids[0].first);
    }
}
