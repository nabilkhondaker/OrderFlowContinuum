# Address / UndefinedBehavior sanitizers

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    message(STATUS "Enabling Address and UndefinedBehavior sanitizers")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()
