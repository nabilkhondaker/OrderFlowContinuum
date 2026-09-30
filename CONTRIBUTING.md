# Contributing to OrderFlow Continuum Lab

Thank you for your interest in contributing.

## Development Setup

1. Clone the repository.
2. Create a build directory and configure with CMake 3.20+.
3. Build and run the test suite (`ctest`).

## Code Style

- C++17/20, follow the provided `.clang-format`.
- Prefer clear, documented interfaces over clever templates.
- All public headers live under `include/ofcl/`.

## Pull Requests

- Include tests for new functionality.
- Update documentation when changing public APIs or experimental protocols.
- Keep commits focused.

## Research Contributions

Experiments, new constitutive models, and failure-case analyses are especially welcome. Please document mathematical assumptions and reproducibility requirements.
