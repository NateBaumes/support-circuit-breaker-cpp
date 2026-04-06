cmake_minimum_required(VERSION 3.20)
project(SupportCircuitBreaker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Main executable
add_executable(support_circuit_breaker
    src/main.cpp
    src/circuit_breaker.cpp
    src/rate_limiter.cpp
    src/vendor_router.cpp
    src/ticket_processor.cpp
    src/http_client.cpp
    include/circuit_breaker.h
    include/rate_limiter.h
    include/vendor_router.h
    include/ticket_processor.h
    include/http_client.h
)

target_include_directories(support_circuit_breaker PRIVATE include)
target_link_libraries(support_circuit_breaker 
    CURL::libcurl
    Threads::Threads
    pthread
)

# Load test executable
add_executable(load_test
    tests/load_test.cpp
    tests/mock_server.cpp
)

target_link_libraries(load_test Threads::Threads)

# Enable optimizations
target_compile_options(support_circuit_breaker PRIVATE -O3 -march=native)
