# Test suite for OrderFlow Continuum Lab

include(GoogleTest)

# Unit tests
add_executable(test_order_book unit/test_order_book.cpp)
target_link_libraries(test_order_book PRIVATE ofcl::core GTest::gtest_main)
gtest_discover_tests(test_order_book)

add_executable(test_deterministic_rng unit/test_deterministic_rng.cpp)
target_link_libraries(test_deterministic_rng PRIVATE ofcl::core GTest::gtest_main)
gtest_discover_tests(test_deterministic_rng)

add_executable(test_continuum_field unit/test_continuum_field.cpp)
target_link_libraries(test_continuum_field PRIVATE ofcl::core GTest::gtest_main)
gtest_discover_tests(test_continuum_field)

# Integration
add_executable(test_hybrid_coupling integration/test_hybrid_coupling.cpp)
target_link_libraries(test_hybrid_coupling PRIVATE ofcl::core GTest::gtest_main)
gtest_discover_tests(test_hybrid_coupling)

# Regression / determinism
add_executable(test_determinism regression/test_determinism.cpp)
target_link_libraries(test_determinism PRIVATE ofcl::core GTest::gtest_main)
gtest_discover_tests(test_determinism)
