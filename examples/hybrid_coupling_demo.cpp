#include "ofcl/core/engine.hpp"
#include <iostream>

int main() {
    ofcl::SimulationConfig cfg;
    cfg.seed = 42;
    ofcl::Engine engine(cfg);
    std::cout << "Hybrid coupling demo — engine constructed successfully.\n";
    return 0;
}
