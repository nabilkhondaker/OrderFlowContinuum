cmake_minimum_required(VERSION 3.20)
project(OrderFlowContinuumLab
    VERSION 0.1.0
    DESCRIPTION "Hybrid discrete LOB + continuum-mechanics high-frequency market microstructure simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(OFCL_BUILD_TESTS "Build unit and integration tests" ON)
option(OFCL_BUILD_EXAMPLES "Build example programs" ON)
option(OFCL_BUILD_PYTHON "Build Python bindings" OFF)
option(OFCL_ENABLE_CUDA "Enable CUDA GPU particle field" OFF)
option(OFCL_ENABLE_SANITIZERS "Enable Address/UB sanitizers" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CompilerWarnings)
include(Dependencies)

if(OFCL_ENABLE_SANITIZERS)
    include(Sanitizers)
endif()

# Version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/ofcl/version.hpp
    @ONLY
)

# Core library
add_library(ofcl_core
    src/core/engine.cpp
    src/core/event.cpp
    src/core/simulation_clock.cpp
    src/core/deterministic_rng.cpp
    src/lob/order_book.cpp
    src/lob/matching_engine.cpp
    src/lob/lockfree_book.cpp
    src/lob/price_level.cpp
    src/lob/book_snapshot.cpp
    src/continuum/field.cpp
    src/continuum/stress_tensor.cpp
    src/continuum/mesh.cpp
    src/continuum/adaptive_mesh.cpp
    src/continuum/sph_solver.cpp
    src/continuum/fem_solver.cpp
    src/continuum/coupling.cpp
    src/latency/latency_model.cpp
    src/latency/network_model.cpp
    src/latency/colocation.cpp
    src/gpu/particle_field.cpp
    src/gpu/gpu_context.cpp
    src/replay/tick_reader.cpp
    src/replay/historical_replay.cpp
    src/replay/synthetic_generator.cpp
    src/metrics/book_metrics.cpp
    src/metrics/latency_metrics.cpp
    src/metrics/continuum_metrics.cpp
    src/visualization/book_heatmap.cpp
    src/visualization/stress_field_viz.cpp
    src/visualization/realtime_dashboard.cpp
    src/experiments/base_experiment.cpp
    src/experiments/baseline.cpp
    src/experiments/discrete_vs_hybrid.cpp
    src/experiments/latency_study.cpp
    src/experiments/failure_analysis.cpp
    src/reporting/report_generator.cpp
    src/reporting/tables.cpp
    src/utils/logging.cpp
    src/utils/timing.cpp
    src/utils/memory.cpp
)

add_library(ofcl::core ALIAS ofcl_core)

target_include_directories(ofcl_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ofcl_core PUBLIC cxx_std_17)

if(OFCL_ENABLE_CUDA)
    enable_language(CUDA)
    target_sources(ofcl_core PRIVATE src/gpu/cuda_kernels.cu)
    target_compile_definitions(ofcl_core PUBLIC OFCL_CUDA_ENABLED)
endif()

# CLI executable
add_executable(ofcl src/cli/main.cpp)
target_link_libraries(ofcl PRIVATE ofcl::core)

# Examples
if(OFCL_BUILD_EXAMPLES)
    add_executable(basic_lob_simulation examples/basic_lob_simulation.cpp)
    target_link_libraries(basic_lob_simulation PRIVATE ofcl::core)

    add_executable(hybrid_coupling_demo examples/hybrid_coupling_demo.cpp)
    target_link_libraries(hybrid_coupling_demo PRIVATE ofcl::core)

    add_executable(tick_replay_example examples/tick_replay_example.cpp)
    target_link_libraries(tick_replay_example PRIVATE ofcl::core)

    add_executable(latency_colocation_example examples/latency_colocation_example.cpp)
    target_link_libraries(latency_colocation_example PRIVATE ofcl::core)

    add_executable(adaptive_mesh_example examples/adaptive_mesh_example.cpp)
    target_link_libraries(adaptive_mesh_example PRIVATE ofcl::core)

    add_executable(gpu_particle_example examples/gpu_particle_example.cpp)
    target_link_libraries(gpu_particle_example PRIVATE ofcl::core)

    add_executable(full_experiment_runner examples/full_experiment_runner.cpp)
    target_link_libraries(full_experiment_runner PRIVATE ofcl::core)
endif()

# Tests
if(OFCL_BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
    # Tests will be added as they are implemented
    add_subdirectory(tests)
endif()

# Install
include(GNUInstallDirs)
install(TARGETS ofcl_core ofcl
    EXPORT ofclTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ofcl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/ofcl/version.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ofcl)

# Package config
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/ofclConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ofclConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ofcl
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ofclConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ofclConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ofclConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ofcl
)

message(STATUS "OrderFlow Continuum Lab ${PROJECT_VERSION}")
message(STATUS "  C++ standard : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Tests        : ${OFCL_BUILD_TESTS}")
message(STATUS "  Examples     : ${OFCL_BUILD_EXAMPLES}")
message(STATUS "  CUDA         : ${OFCL_ENABLE_CUDA}")
