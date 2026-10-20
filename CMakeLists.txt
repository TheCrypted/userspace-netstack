cmake_minimum_required(VERSION 3.20)

project(NetStack VERSION 1.0 LANGUAGES CXX)

add_executable(network_stack
    src/main.cpp
    src/tun_device.cpp
    src/tun_syscalls.cpp
    src/ipv4.cpp
    include/tun_device.hpp
    include/unique_fd.hpp
    include/ipv4.hpp
)

target_compile_features(network_stack PRIVATE cxx_std_20)

set_target_properties(network_stack PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(network_stack PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

include(CTest)

if (BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(netstack_tests
        tests/unit/ipv4_test.cpp
        tests/unit/unique_fd_test.cpp
        tests/unit/tun_device_test.cpp
        src/ipv4.cpp
        src/tun_device.cpp
        src/tun_syscalls.cpp
    )

    target_compile_features(netstack_tests PRIVATE cxx_std_20)
    set_target_properties(netstack_tests PROPERTIES CXX_EXTENSIONS OFF)
    target_include_directories(netstack_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories(netstack_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests/support")
    target_link_libraries(netstack_tests PRIVATE GTest::gtest_main GTest::gmock)

    include(GoogleTest)
    gtest_discover_tests(netstack_tests)
endif()
