cmake_minimum_required(VERSION 3.20)

project(NetStack VERSION 1.0 LANGUAGES CXX)

add_executable(app
    src/main.cpp
    src/tun_device.cpp
    src/ipv4.cpp
    include/tun_device.hpp
    include/unique_fd.hpp
    include/ipv4.hpp
)

target_compile_features(app PRIVATE cxx_std_20)

set_target_properties(app PROPERTIES
    CXX_EXTENSIONS OFF
)

target_include_directories(app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
