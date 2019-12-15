# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.3)
project (seqan3_test_external_project CXX)

include (../seqan3-test.cmake) # for SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS, SEQAN3_VERSION
include (ExternalProject)

set (SEQAN3_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../")

# install and package seqan3 library
ExternalProject_Add (
    seqan3_test_prerequisite
    PREFIX seqan3_test_prerequisite
    SOURCE_DIR "${SEQAN3_ROOT}"
    CMAKE_ARGS
        ${SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/usr"
    STEP_TARGETS configure install
    BUILD_BYPRODUCTS "<BINARY_DIR>/include"
)
set (SEQAN3_PACKAGE_ZIP_URL "${PROJECT_BINARY_DIR}/seqan3-${SEQAN3_VERSION}-${CMAKE_SYSTEM_NAME}.zip")
ExternalProject_Add_Step (
    seqan3_test_prerequisite
    package
    COMMAND ${CMAKE_CPACK_COMMAND} -G ZIP -B "${PROJECT_BINARY_DIR}"
    DEPENDEES configure install
    WORKING_DIRECTORY "<BINARY_DIR>"
    BYPRODUCTS
        ${SEQAN3_PACKAGE_ZIP_URL}
        "${SEQAN3_PACKAGE_ZIP_URL}.sha256"
)
ExternalProject_Get_property(seqan3_test_prerequisite BINARY_DIR)
set (SEQAN3_SYSTEM_PREFIX "${BINARY_DIR}/usr")

# 1) find_package: seqan3 is a submodule and we add it with add_subdirectory
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    seqan3_submodule_add_subdirectory
    PREFIX seqan3_submodule_add_subdirectory
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/seqan3_submodule_add_subdirectory"
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
