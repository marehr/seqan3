# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.4)
project (seqan3_test_external_project CXX)

include (../seqan3-test.cmake) # for SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS, SEQAN3_VERSION
include (ExternalProject)

set (SEQAN3_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../")
include (install-seqan3.cmake)
set (SEQAN3_EXTERNAL_PROJECT_CMAKE_COMMAND "${CMAKE_COMMAND}")
if (NOT ${CMAKE_VERSION} VERSION_LESS 3.14)
    include (use-cmake3.4.cmake) # ensure that seqan3 can be used with cmake 3.4
endif ()

# 1) find_package: seqan3 is a submodule and we add it with add_subdirectory
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    seqan3_submodule_add_subdirectory
    PREFIX seqan3_submodule_add_subdirectory
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/seqan3_submodule_add_subdirectory"
    CMAKE_COMMAND "${SEQAN3_EXTERNAL_PROJECT_CMAKE_COMMAND}"
    CMAKE_ARGS
        ${SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS}
        "-DSEQAN3_ROOT=${SEQAN3_ROOT}"
)

# 2) find_package: seqan3 is a submodule and we only specify the path to `seqan3-config.cmake`
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    seqan3_submodule_find_package
    PREFIX seqan3_submodule_find_package
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/seqan3_submodule_find_package"
    CMAKE_COMMAND "${SEQAN3_EXTERNAL_PROJECT_CMAKE_COMMAND}"
    CMAKE_ARGS
        ${SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS}
        "-DCMAKE_PREFIX_PATH=${SEQAN3_ROOT}/build_system"
)

# 3) find_package: seqan3 is system-wide installed / PREFIX_INSTALL (unzipped)
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    seqan3_installed
    PREFIX seqan3_installed
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/seqan3_installed"
    CMAKE_COMMAND "${SEQAN3_EXTERNAL_PROJECT_CMAKE_COMMAND}"
    CMAKE_ARGS
        ${SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS}
        "-DCMAKE_SYSTEM_PREFIX_PATH=${SEQAN3_SYSTEM_PREFIX}"
)
add_dependencies(seqan3_installed seqan3_test_prerequisite)

# 4) find_package: seqan3 was downloaded as an archive file via fetch content (e.g. zip, tar.xz)
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
if (NOT ${CMAKE_VERSION} VERSION_LESS 3.14)
    ExternalProject_Add (
        seqan3_fetch_content_zip
        PREFIX seqan3_fetch_content_zip
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/seqan3_fetch_content_zip"
        CMAKE_ARGS
            ${SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS}
            "-DSEQAN3_PACKAGE_ZIP_URL=${SEQAN3_PACKAGE_ZIP_URL}"
    )
    add_dependencies(seqan3_fetch_content_zip seqan3_test_prerequisite)
endif ()
