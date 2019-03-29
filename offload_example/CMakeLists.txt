
cmake_minimum_required (VERSION 3.2...3.12)
project (offload_app CXX)

set (SeqAn3_DIR "${CMAKE_SOURCE_DIR}/../build_system/")
find_package (SeqAn3)

add_executable(sum_serial sum_serial.cpp)
target_link_libraries (sum_serial seqan3::seqan3)

add_executable(sum_thread sum_thread.cpp)
target_link_libraries (sum_thread seqan3::seqan3)

add_executable(sum_simd sum_simd.cpp)
target_link_libraries (sum_simd seqan3::seqan3)
target_compile_options (sum_simd PUBLIC -march=native)

add_executable(offload_example offload_example.cpp)
target_link_libraries (offload_example seqan3::seqan3 seqan3::mpi_app)

add_executable(sum_offload sum_offload.cpp)
target_link_libraries (sum_offload seqan3::seqan3 seqan3::mpi_app)
