# Architecture

## Overview

OrderFlow Continuum Lab is organized around a hybrid simulation engine that keeps a discrete limit-order-book as the source of truth for matching while maintaining a continuum field representation of liquidity and order-flow stress.

## Major Subsystems

1. **Core** — simulation clock, deterministic RNG, event types, central Engine.
2. **LOB** — price-time priority order book, matching engine, experimental lock-free side.
3. **Continuum** — field, stress tensor, mesh, adaptive refinement, SPH (primary) and FEM (experimental) solvers, coupling layer.
4. **Latency** — matching-engine, network, co-location models.
5. **GPU** — optional particle field (CUDA with CPU fallback).
6. **Replay** — synthetic generator and historical tick readers.
7. **Metrics** — book, latency, continuum, impact, determinism checks.
8. **Visualization** — heatmaps, stress fields, mesh, particles, dashboard.
9. **Experiments** — named experimental protocols.
10. **Reporting** — tables and research report generation.
11. **CLI** — `ofcl` entry point.

## Data Flow

Events enter via the CLI or experiment runner, are delayed according to the latency model, and are processed by the matching engine. Periodically the continuum coupling projects the book state onto the field, advances the SPH/FEM solver, and optionally refines the mesh. Metrics collectors and visualization modules observe both discrete and continuum state.

## Lock-Free Considerations

The primary `OrderBook` uses a mutex for writer safety. An experimental `LockFreeBookSide` provides atomic level publishing for concurrent readers. Full multi-producer lock-free matching is an active research direction; the current design prioritizes determinism and correctness over maximum concurrency.
