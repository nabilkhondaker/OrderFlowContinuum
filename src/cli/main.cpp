#include "ofcl/core/engine.hpp"
#include "ofcl/metadata.hpp"
#include "ofcl/core/deterministic_rng.hpp"
#include "ofcl/lob/order_book.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

namespace {

void print_help() {
    std::cout <<
R"(OrderFlow Continuum Lab (ofcl) — hybrid discrete/continuum LOB simulator
Author: Nabil Khondaker

Usage:
  ofcl <command> [options]

Commands:
  generate-scenario   Generate synthetic tick / order-flow data
  run-simulation      Run a hybrid discrete + continuum simulation
  replay-ticks        Replay historical or synthetic ticks
  benchmark           Throughput / latency micro-benchmarks
  experiment          Run a named experiment from the suite
  report              Generate a research report for an experiment
  visualize           Produce visualization artifacts
  version             Print version information
  help                Show this help

Examples:
  ofcl run-simulation --seed 42
  ofcl experiment --name baseline
  ofcl benchmark
)";
}

void cmd_version() {
    std::cout << ofcl::ProjectMetadata::name << "\n"
              << "Author: " << ofcl::ProjectMetadata::author << "\n"
              << "Description: " << ofcl::ProjectMetadata::description << "\n";
}

void cmd_run_simulation(int argc, char** argv) {
    ofcl::SimulationConfig cfg;
    cfg.seed = 42;
    cfg.threads = 1;
    cfg.deterministic = true;

    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            cfg.seed = std::stoull(argv[++i]);
        }
    }

    std::cout << "[ofcl] Initializing hybrid simulation (seed=" << cfg.seed << ")\n";
    ofcl::Engine engine(cfg);

    // Inject a small synthetic scenario
    ofcl::DeterministicRng rng(cfg.seed);
    ofcl::TimestampNs t = 0;
    for (int i = 0; i < 1000; ++i) {
        ofcl::Event e;
        e.type = ofcl::EventType::NewOrder;
        e.timestamp = t;
        e.sequence = static_cast<std::uint64_t>(i);
        ofcl::NewOrderEvent no;
        no.order.id = 0;
        no.order.side = (rng.uniform01() < 0.5) ? ofcl::Side::Bid : ofcl::Side::Ask;
        no.order.type = ofcl::OrderType::Limit;
        no.order.price = 100.0 + rng.uniform(-1.0, 1.0);
        no.order.quantity = rng.uniform(1.0, 100.0);
        no.order.remaining = no.order.quantity;
        no.order.timestamp = t;
        e.payload = no;
        engine.submit(e);
        t += static_cast<ofcl::TimestampNs>(rng.uniform(100.0, 5000.0));
    }

    std::cout << "[ofcl] Running simulation...\n";
    engine.run(1000);
    auto snap = engine.book().snapshot(10);
    std::cout << "[ofcl] Final book: "
              << snap.bids.size() << " bid levels, "
              << snap.asks.size() << " ask levels\n";
    if (!snap.bids.empty() && !snap.asks.empty()) {
        std::cout << "[ofcl] Mid ≈ " << snap.mid
                  << "  imbalance=" << snap.imbalance << "\n";
    }
    std::cout << "[ofcl] Simulation complete.\n";
}

void cmd_benchmark() {
    std::cout << "[ofcl] Running micro-benchmark (order insertion + match)...\n";
    ofcl::OrderBook book(0.01, 50);
    ofcl::DeterministicRng rng(42);
    const int N = 100000;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        ofcl::Order o;
        o.id = static_cast<ofcl::OrderId>(i + 1);
        o.side = (i % 2 == 0) ? ofcl::Side::Bid : ofcl::Side::Ask;
        o.type = ofcl::OrderType::Limit;
        o.price = 100.0 + rng.uniform(-2.0, 2.0);
        o.quantity = rng.uniform(1.0, 50.0);
        o.remaining = o.quantity;
        book.add(o);
    }
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "[ofcl] Inserted " << N << " orders in " << ms << " ms ("
              << (N / (ms / 1000.0)) << " orders/s)\n";
    std::cout << "[ofcl] Book order count: " << book.order_count() << "\n";
}

void cmd_experiment(int argc, char** argv) {
    std::string name = "baseline";
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "--name") == 0 && i + 1 < argc) {
            name = argv[++i];
        }
    }
    std::cout << "[ofcl] Experiment '" << name << "'\n";
    std::cout << "[ofcl] Results not yet generated. Run the corresponding experiment "
                 "to populate metrics.\n";
    std::cout << "[ofcl] (Skeleton: implement full experiment runners under "
                 "src/experiments/)\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }
    std::string cmd = argv[1];
    if (cmd == "help" || cmd == "-h" || cmd == "--help") {
        print_help();
    } else if (cmd == "version" || cmd == "--version") {
        cmd_version();
    } else if (cmd == "run-simulation") {
        cmd_run_simulation(argc, argv);
    } else if (cmd == "benchmark") {
        cmd_benchmark();
    } else if (cmd == "experiment") {
        cmd_experiment(argc, argv);
    } else if (cmd == "generate-scenario" || cmd == "replay-ticks" ||
               cmd == "report" || cmd == "visualize") {
        std::cout << "[ofcl] Command '" << cmd
                  << "' is declared; full implementation pending expansion of "
                     "the research skeleton.\n";
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
        print_help();
        return 1;
    }
    return 0;
}
