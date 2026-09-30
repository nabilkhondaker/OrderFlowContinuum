#pragma once

#include "ofcl/types.hpp"
#include "ofcl/core/event.hpp"
#include "ofcl/core/simulation_clock.hpp"
#include "ofcl/core/deterministic_rng.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace ofcl {

// Forward declarations
class OrderBook;
class MatchingEngine;
class ContinuumCoupling;
class LatencyModel;
class MetricsCollector;

struct SimulationConfig {
    TimestampNs resolution_ns{1};
    int threads{1};
    bool deterministic{true};
    std::uint64_t seed{42};
    double tick_size{0.01};
    int max_depth{100};
    std::string continuum_solver{"sph"};  // "sph" or "fem"
    bool adaptive_mesh{true};
    double refinement_threshold{0.05};
    bool gpu_enabled{false};
    int gpu_device{0};
    TimestampNs matching_engine_latency_ns{1200};
    std::string network_model{"lognormal"};
    bool colocation{false};
};

/// Central hybrid simulation engine.
/// Owns the discrete LOB, continuum coupling, latency model and metrics.
class Engine {
public:
    explicit Engine(SimulationConfig cfg = {});
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void reset();
    void load_config(const SimulationConfig& cfg);

    /// Inject a single event (applies latency model then queues for matching).
    void submit(const Event& e);

    /// Process all pending events up to (and including) the given time.
    void step_until(TimestampNs t);

    /// Run for a fixed number of events or until the event source is exhausted.
    void run(std::size_t max_events = 0);

    // Accessors for inspection / visualization
    const OrderBook& book() const;
    SimulationClock& clock() { return clock_; }
    const SimulationClock& clock() const { return clock_; }
    DeterministicRng& rng() { return rng_; }
    const SimulationConfig& config() const { return cfg_; }

    using EventCallback = std::function<void(const Event&)>;
    void set_event_callback(EventCallback cb) { event_cb_ = std::move(cb); }

private:
    SimulationConfig cfg_;
    SimulationClock clock_;
    DeterministicRng rng_;
    std::unique_ptr<OrderBook> book_;
    std::unique_ptr<MatchingEngine> matcher_;
    std::unique_ptr<ContinuumCoupling> continuum_;
    std::unique_ptr<LatencyModel> latency_;
    std::unique_ptr<MetricsCollector> metrics_;
    EventCallback event_cb_;
    std::vector<Event> pending_;
};

}  // namespace ofcl
