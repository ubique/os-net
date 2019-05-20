cmake_minimum_required(VERSION 3.2)

project(net)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(3rd_party)

find_package(Boost 1.59.0 REQUIRED COMPONENTS system filesystem unit_test_framework program_options timer)
add_definitions(-DBOOST_ALL_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/3rd_party/eggs-variant/include)
add_definitions(-DEGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE -DEGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
endif()

add_executable(net
    main.cpp
    util.hpp
    server.h server.cpp
    client_manager.h client_manager.cpp
)

add_executable(net_asan
    main.cpp
    util.hpp
    server.h server.cpp
    client_manager.h client_manager.cpp
)

add_compile_options(jit_asan PUBLIC -fsanitize=address)
add_link_options(jit_asan -lasan)

