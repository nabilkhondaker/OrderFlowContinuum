#include <gtest/gtest.h>
#include "ofcl/lob/order_book.hpp"
#include "ofcl/lob/matching_engine.hpp"

using namespace ofcl;

TEST(OrderBook, AddAndBestBidAsk) {
    OrderBook book(0.01, 20);
    Order o;
    o.id = 1;
    o.side = Side::Bid;
    o.type = OrderType::Limit;
    o.price = 100.0;
    o.quantity = 10.0;
    o.remaining = 10.0;
    EXPECT_TRUE(book.add(o));

    o.id = 2;
    o.side = Side::Ask;
    o.price = 100.05;
    EXPECT_TRUE(book.add(o));

    auto bb = book.best_bid();
    auto ba = book.best_ask();
    ASSERT_TRUE(bb.has_value());
    ASSERT_TRUE(ba.has_value());
    EXPECT_DOUBLE_EQ(*bb, 100.0);
    EXPECT_DOUBLE_EQ(*ba, 100.05);
}

TEST(OrderBook, MatchMarket) {
    OrderBook book(0.01, 20);
    Order resting;
    resting.id = 1;
    resting.side = Side::Ask;
    resting.type = OrderType::Limit;
    resting.price = 100.0;
    resting.quantity = 5.0;
    resting.remaining = 5.0;
    book.add(resting);

    auto trades = book.match_market(Side::Bid, 3.0, 99, 1000);
    ASSERT_EQ(trades.size(), 1u);
    EXPECT_DOUBLE_EQ(trades[0].quantity, 3.0);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

TEST(MatchingEngine, ProcessNewOrder) {
    OrderBook book(0.01, 20);
    MatchingEngine me(book);
    Event e;
    e.type = EventType::NewOrder;
    e.timestamp = 100;
    NewOrderEvent no;
    no.order.id = 0;
    no.order.side = Side::Bid;
    no.order.type = OrderType::Limit;
    no.order.price = 99.5;
    no.order.quantity = 10.0;
    no.order.remaining = 10.0;
    e.payload = no;
    auto trades = me.process(e);
    EXPECT_TRUE(trades.empty());
    EXPECT_EQ(book.order_count(), 1u);
}
