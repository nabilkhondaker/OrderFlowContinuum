# OrderFlow Continuum Lab

**Can continuum mechanics model high-frequency order flow?**

A research-oriented, lock-free, multi-threaded C++17/20 engine that simulates a full limit-order-book exchange at nanosecond resolution and couples it to a continuum-mechanics solver treating liquidity as a deformable continuum with stress and strain fields.

```
Tick / Order Events
        │
        ▼
Lock-free LOB Engine (ns resolution)
        │
        ▼
Order-flow Pressure + Liquidity Stress Tensors
        │
        ▼
Continuum Solver (SPH / FEM + Adaptive Mesh)
        │
        ▼
GPU Particle Field (optional) ──► Real-time Stress / Strain Visualization + Metrics
```

📢 **Release Notice:** This repository contains the complete codebase for this project, engineered between *September 7, 2025* and *September 30, 2026*. The work was developed intermittently alongside other research software and has been packaged in full for public viewing and use. Relative to my other projects this one is more ambitious in scope — a hybrid discrete limit-order-book plus continuum layer instead of a single-domain simulator — but the honest reason it exists is practical: I built it to deepen my own understanding of order flow and microstructure while day trading, not to ship a trading product or claim an edge.

**Author:** Nabil Khondaker

[![C++](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](.github/workflows/build.yml)

---

## Overview

OrderFlow Continuum Lab (OFCL) investigates a hybrid discrete–continuum formulation of high-frequency market microstructure. A classical lock-free limit-order-book engine operates at nanosecond resolution. In parallel, order-flow intensity, imbalance, and depth are projected onto a continuum field whose evolution is governed by constitutive relations inspired by continuum mechanics. Liquidity is treated as a deformable medium whose stress and strain tensors respond to discrete order events.

The project does **not** assume that continuum coupling is superior. It provides the infrastructure to measure fidelity, computational cost, determinism, and latency characteristics against a pure discrete baseline under controlled experimental conditions.

### Research Question

> Can a hybrid discrete limit-order-book + continuum-mechanics formulation (treating order-flow pressure and liquidity as stress/strain fields on a deformable continuum) accurately and usefully model high-frequency market microstructure dynamics at nanosecond resolution, and how does its fidelity, computational cost, determinism, and latency characteristics compare with pure discrete event-driven LOB simulators?

---

## Motivation

Classical discrete LOB simulators capture exact matching semantics and priority but scale poorly when one wishes to reason about macroscopic liquidity surfaces, stress concentrations, or adaptive spatial discretizations. Continuum models offer elegant field representations and mesh-adaptation machinery but lose the discrete event semantics that dominate at the exchange matching-engine level.

A hybrid approach keeps the discrete LOB as the source of truth for matching while using a continuum layer for:

- order-flow pressure and liquidity stress visualization,
- adaptive mesh refinement driven by local imbalance gradients,
- GPU-accelerated particle representations of aggressive flow,
- controlled experiments on co-location and latency distributions.

---

## Why Continuum Mechanics for Order Flow?

In continuum mechanics a stress tensor \(\boldsymbol{\sigma}\) relates internal forces to surface orientation. Analogously, one may define an order-flow stress whose components encode:

- intensity of aggressive market orders,
- standing liquidity depth,
- cancellation pressure,
- imbalance between bid and ask sides.

The resulting field can be evolved with SPH or finite-element discretizations, refined adaptively where gradients are large, and visualized in real time. The discrete LOB remains authoritative for price formation; the continuum layer supplies a complementary macroscopic view and a set of quantitative residuals that measure consistency between the two representations.

---

## Key Features

- **Lock-free Limit Order Book** – price-time priority, multiple order types, nanosecond timestamps, concurrent access patterns designed to avoid false sharing.
- **Continuum Coupling** – SPH (primary) and FEM (experimental) solvers, liquidity surface, stress/pressure tensors, adaptive mesh refinement.
- **Latency & Co-location Models** – configurable network / matching-engine latency distributions, co-location advantage, queue-position effects.
- **GPU Particle Field** – optional CUDA / CPU-fallback particle representation of order flow.
- **Tick Replay & Synthetic Generation** – deterministic historical replay and reproducible synthetic generators.
- **Experiment Suite** – baseline hybrid, discrete-vs-hybrid, latency sensitivity, mesh refinement, GPU scaling, thin-book shocks, determinism checks, failure-case analysis.
- **Metrics & Reporting** – latency percentiles, throughput, book-state residuals, stress consistency, determinism verification.
- **Visualization** – book-depth heatmaps, stress-field overlays, adaptive-mesh diagnostics, particle fields.
- **Configuration-driven** – YAML configuration for all major parameters; no hard-coded experiment constants.
- **Reproducibility** – seeded RNGs, configuration snapshots, experiment metadata.

---

## Mathematical Background

### Discrete Limit Order Book

Orders arrive as events \(e = (t, \text{side}, \text{price}, \text{size}, \text{type}, \ldots)\) with nanosecond timestamps. Matching follows price-time priority. The book state at any instant is a pair of sorted maps of price levels.

### Order-Flow Pressure and Liquidity Stress

Define a scalar pressure field \(p(x,t)\) and a stress tensor \(\boldsymbol{\sigma}(x,t)\) on a one-dimensional liquidity surface parameterized by price \(x\):

\[
p(x,t) = \alpha\, I(x,t) + \beta\, D(x,t) + \gamma\, C(x,t)
\]

where \(I\) is aggressive order intensity, \(D\) is standing depth, and \(C\) is cancellation rate. The stress tensor is constructed from imbalance gradients and may be evolved with a constitutive model of the form

\[
\frac{D\boldsymbol{\sigma}}{Dt} = \mathbf{f}(\boldsymbol{\sigma}, \nabla\mathbf{v}, \mathbf{D})
\]

with \(\mathbf{v}\) a macroscopic flow velocity derived from net order flow.

### Continuum Formulation (SPH / FEM)

SPH is the primary solver: particles carry mass (liquidity), pressure, and stress. Kernel interpolation yields continuum fields. FEM is provided as an experimental alternative on a dynamically refined mesh.

### Adaptive Mesh Refinement

Refinement is triggered when

\[
\|\nabla p\| > \theta_{\text{ref}} \quad\text{or}\quad \|\boldsymbol{\sigma}\| > \theta_{\sigma}.
\]

Coarsening occurs when both quantities fall below hysteresis thresholds.

### Latency and Co-location

Latency is modeled as

\[
L = L_{\text{network}} + L_{\text{ME}} + L_{\text{queue}}
\]

with log-normal or empirical distributions. Co-location reduces the network component for privileged participants.

### Hybrid Coupling

Discrete events update the LOB immediately. Periodically (or on event batches) the continuum field is advanced using the latest book snapshot and recent order-flow statistics. Residuals between discrete depth and continuum-reconstructed depth quantify coupling fidelity.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      CLI / Experiment Runner                │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│                     Simulation Engine                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Event Queue  │  │ LOB Engine   │  │ Continuum Solver │  │
│  │ (lock-free)  │──│ (lock-free)  │──│ (SPH / FEM)      │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
│         │                   │                   │           │
│  ┌──────▼──────┐   ┌────────▼────────┐  ┌───────▼────────┐ │
│  │ Latency     │   │ Metrics         │  │ GPU Particles  │ │
│  │ Model       │   │ Collectors      │  │ (optional)     │ │
│  └─────────────┘   └─────────────────┘  └────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Lock-Free Design

- Single-producer / multi-consumer or multi-producer rings for event ingress.
- Cache-line-aligned price-level nodes.
- Atomic sequence numbers and versioned snapshots for readers.
- Deterministic event ordering under controlled seeds.

### Cache-Aware Layouts

Critical structures are padded to cache-line boundaries. Hot paths avoid false sharing between matching threads and continuum update threads.

---

## Build & Installation

### Requirements

- CMake ≥ 3.20
- C++17 (C++20 preferred)
- Optional: CUDA toolkit (GPU particle field)
- Optional: Python 3.9+ with pybind11 (bindings)
- Google Test (fetched by CMake or system)

### Quick Start

```bash
git clone <repo-url> orderflow-continuum-lab
cd orderflow-continuum-lab
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
ctest --output-on-failure
```

### Configuration

All major parameters live in YAML under `configs/`. Example:

```yaml
simulation:
  resolution_ns: 1
  threads: 8
  deterministic: true

lob:
  tick_size: 0.01
  max_depth: 100

continuum:
  solver: sph
  adaptive_mesh: true
  refinement_threshold: 0.05

latency:
  matching_engine_ns: 1200
  network_model: lognormal
  colocation: true

gpu:
  enabled: false
  device: 0

reproducibility:
  seed: 42
```

---

## CLI

After building, the `ofcl` binary provides:

```bash
./ofcl generate-scenario --config configs/default.yaml --output data/synthetic/
./ofcl run-simulation    --config configs/default.yaml
./ofcl replay-ticks      --input data/sample_ticks/ --config configs/historical_replay.yaml
./ofcl benchmark         --config configs/benchmark.yaml
./ofcl experiment        --name baseline
./ofcl experiment        --name discrete_vs_hybrid
./ofcl report            --experiment baseline
./ofcl visualize         --run results/latest/
```

---

## Experiments

| ID | Name                        | Purpose                                      |
|----|-----------------------------|----------------------------------------------|
| 1  | Baseline Hybrid             | Full hybrid system on synthetic + replay data |
| 2  | Discrete vs Hybrid          | Fidelity and cost comparison                 |
| 3  | Latency & Co-location       | Sensitivity to latency distributions         |
| 4  | Adaptive Mesh Refinement    | Accuracy vs cost under different criteria    |
| 5  | GPU Particle Field Scaling  | CPU-only vs GPU                              |
| 6  | Thin-Book / Liquidity Shock | Extreme regimes                              |
| 7  | Determinism & Reproducibility | Seeded identical trajectories              |
| 8  | Tick Replay Fidelity        | Historical data accuracy                     |
| 9  | Scalability                 | Threads / cores / NUMA                       |
| 10 | Visualization Correctness   | Stress-field consistency                     |
| 11 | Failure Case Analysis       | Singularities, voids, residuals              |

Results are written under `experiments/results/`. No fabricated numbers appear in the repository; run the corresponding experiment to populate metrics.

---

## Testing

```bash
cd build
ctest --output-on-failure
# or selectively
./tests/unit/test_order_book
./tests/integration/test_hybrid_coupling
./tests/performance/test_throughput
```

Coverage includes order-book correctness, lock-free stress tests, continuum field updates, adaptive-mesh triggers, latency distributions, determinism, and end-to-end hybrid pipelines.

---

## Project Structure

See the repository tree. Major subsystems live under `include/ofcl/` and `src/`. Documentation is in `docs/`. Experiment configurations are under `experiments/configs/`.

---

## Reproducibility

- All RNGs are seeded from configuration.
- Simulation clock is deterministic under the same seed and configuration.
- Experiment runners write a metadata snapshot (compiler, flags, seed, config hash).
- Determinism regression tests assert identical book trajectories for fixed seeds.

---

## Limitations

- Continuum constitutive relations are phenomenological; they are not derived from first-principles market microstructure theory.
- Adaptive mesh refinement criteria are heuristic.
- GPU path is optional and currently limited to particle advection / stress evaluation.
- Full exchange matching-engine fidelity (e.g., self-trade prevention, complex order types) is simplified relative to production systems.
- Large-scale historical tick datasets are not shipped; only sample data is included.

---

## Future Research

- Learned constitutive models conditioned on regime.
- Multi-asset / cross-impact continuum fields.
- Formal verification of lock-free invariants.
- Integration with real market-data feeds under controlled co-location latency models.
- Uncertainty quantification of continuum parameters.

---

## License

MIT License. See [LICENSE](LICENSE).

---

## Citation

```bibtex
@software{khondaker_orderflow_continuum_lab,
  author = {Khondaker, Nabil},
  title  = {OrderFlow Continuum Lab},
  year   = {2026},
  url    = {https://github.com/nabilkhondaker/orderflow-continuum-lab}
}
```

See also `CITATION.cff`.

---

*OrderFlow Continuum Lab — hybrid discrete/continuum investigation of high-frequency order flow.*
