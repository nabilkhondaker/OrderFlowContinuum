# External dependencies for OrderFlow Continuum Lab
# Prefer FetchContent for optional/test deps to keep the build self-contained.

include(FetchContent)

# GoogleTest for unit tests
if(OFCL_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Note: Eigen, spdlog, yaml-cpp etc. can be added here when full
# continuum solvers and configuration parsing are expanded.
# For the current research skeleton we keep the dependency surface minimal
# and rely on the C++ standard library + carefully chosen lock-free patterns.
