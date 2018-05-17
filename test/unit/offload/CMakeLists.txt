
find_package(MPI REQUIRED)
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

# Import ham_offload_mpi
set(HAM_CLONE_DIR "${SEQAN3_CLONE_DIR}/submodules/ham")
add_subdirectory (${HAM_CLONE_DIR} ${CMAKE_BINARY_DIR}/submodules/ham)

# ham's standard requirement for c++11 conflicts with seqan's requirement for >=c++17
# See related problems
# https://gitlab.kitware.com/cmake/cmake/issues/17146
# https://gitlab.kitware.com/cmake/cmake/issues/16608
set_property(TARGET ham_interface PROPERTY INTERFACE_COMPILE_FEATURES "")

add_library(gtest_main_mpi STATIC gtest_main_mpi.cpp)
# target_include_directories (gtest_main_mpi PRIVATE ${SEQAN3_INCLUDE_DIRS} ${SEQAN3_TEST_INCLUDE_DIRS})
target_link_libraries (gtest_main_mpi PRIVATE seqan3::test::unit)
target_include_directories (gtest_main_mpi PRIVATE ${MPI_CXX_INCLUDE_PATH})
add_dependencies (gtest_main_mpi gtest)

macro (seqan3_mpi_test target_cpp)
    # separate_arguments (cxx_flags_list UNIX_COMMAND "${SEQAN3_CXX_FLAGS}")

    # $target_cpp = "pod_tuple_test.cpp"
    #   * will register the global TARGET name "pod_tuple_test" and
    #   * will register the test case name "core_pod_tuple" if
    #     pod_tuple_test.cpp is in test/core/
    #
    # NOTE(marehr): ".+/test/" REGEX is greedy, that means
    # /test/test/test/hello_test.cpp will result in an empty `test_path`
    string (REGEX REPLACE "_test.cpp$" "" target_name ${target_cpp})
    string (REGEX REPLACE ".+/test/" "" test_path ${CMAKE_CURRENT_LIST_DIR})
    set (target "${target_name}_test")

    add_executable (${target} ${target_cpp})
    target_link_libraries (${target} seqan3::test::unit)
    # target_compile_options (${target} PRIVATE ${cxx_flags_list} ${SEQAN3_TEST_CXX_FLAGS})
    # target_compile_definitions (${target} PRIVATE ${SEQAN3_DEFINITIONS} ${SEQAN3_TEST_DEFINITIONS})
    # target_include_directories (${target} PRIVATE ${SEQAN3_INCLUDE_DIRS} ${SEQAN3_TEST_INCLUDE_DIRS})
    # target_link_libraries (${target} ${SEQAN3_LIBRARIES} gtest pthread)
    add_test (NAME "${test_path}/${target}" COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS} ${target} ${MPIEXEC_POSTFLAGS})
    add_dependencies (${target} gtest)

    target_compile_options (${target} PRIVATE "-Wno-error")

    target_include_directories(${target} PRIVATE ${MPI_CXX_INCLUDE_PATH})
    target_link_libraries(${target} gtest_main_mpi ham_offload_mpi ${MPI_CXX_LIBRARIES} ${Boost_LIBRARIES})

    unset (cxx_flags_list)
    unset (target_name)
    unset (test_path)
    unset (target)
endmacro ()

seqan3_mpi_test (mpi_backend_test.cpp)
seqan3_mpi_test (mpi_gtest_test.cpp)

seqan3_mpi_test (sized_buffer_ptr_test.cpp)
seqan3_mpi_test (target_migratable_test.cpp)
seqan3_mpi_test (target_function_test.cpp)

seqan3_mpi_test (contiguous_container_test.cpp)

seqan3_mpi_test (mpi_offload_test.cpp)
seqan3_mpi_test (mpi_offload_executor_test.cpp)
seqan3_mpi_test (mpi_offload_twoway_executor_test.cpp)
