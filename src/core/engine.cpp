#include "ofcl/core/engine.hpp"
#include "ofcl/lob/order_book.hpp"
#include "ofcl/lob/matching_engine.hpp"
#include "ofcl/continuum/coupling.hpp"
#include "ofcl/latency/latency_model.hpp"
#include <algorithm>
#include <iostream>

namespace ofcl {

// Minimal metrics collector placeholder
class MetricsCollector {
public:
    void record_trade(const Trade&) {}
    void record_latency(TimestampNs) {}
};

Engine::Engine(SimulationConfig cfg) : cfg_(std::move(cfg)), rng_(cfg_.seed) {
    book_ = std::make_unique<OrderBook>(cfg_.tick_size, cfg_.max_depth);
    matcher_ = std::make_unique<MatchingEngine>(*book_);
    continuum_ = std::make_unique<ContinuumCoupling>(
        cfg_.continuum_solver, cfg_.adaptive_mesh, cfg_.refinement_threshold);
    latency_ = std::make_unique<LatencyModel>(
        cfg_.matching_engine_latency_ns, cfg_.network_model, cfg_.colocation);
    metrics_ = std::make_unique<MetricsCollector>();
}

Engine::~Engine() = default;

void Engine::reset() {
    book_->clear();
    clock_.set(0);
    pending_.clear();
    rng_.seed(cfg_.seed);
}

void Engine::load_config(const SimulationConfig& cfg) {
    cfg_ = cfg;
    reset();
}

void Engine::submit(const Event& e) {
    Event delayed = e;
    TimestampNs lat = latency_->sample(rng_, cfg_.colocation);
    delayed.timestamp = e.timestamp + lat;
    pending_.push_back(delayed);
    std::push_heap(pending_.begin(), pending_.end(),
                   [](const Event& a, const Event& b) {
                       return a.timestamp > b.timestamp;
                   });
}

void Engine::step_until(TimestampNs t) {
    while (!pending_.empty()) {
        std::pop_heap(pending_.begin(), pending_.end(),
                      [](const Event& a, const Event& b) {
                          return a.timestamp > b.timestamp;
                      });
        Event e = pending_.back();
        if (e.timestamp > t) {
            pending_.push_back(e);
            std::push_heap(pending_.begin(), pending_.end(),
                           [](const Event& a, const Event& b) {
                               return a.timestamp > b.timestamp;
                           });
            break;
        }
        pending_.pop_back();
        clock_.advance_to(e.timestamp);
        auto trades = matcher_->process(e);
        for (const auto& tr : trades) metrics_->record_trade(tr);
        if (event_cb_) event_cb_(e);

        if (clock_.now() % 10000 == 0) {
            auto snap = book_->snapshot(20);
            continuum_->update_from_snapshot(snap, 1.0);
            continuum_->step(1e-6);
        }
    }
    clock_.advance_to(t);
}

void Engine::run(std::size_t max_events) {
    std::size_t processed = 0;
    while (!pending_.empty() && (max_events == 0 || processed < max_events)) {
        step_until(pending_.front().timestamp);
        ++processed;
    }
}

const OrderBook& Engine::book() const { return *book_; }

}  // namespace ofcl
