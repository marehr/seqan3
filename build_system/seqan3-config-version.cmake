# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------

# This file adds version support for `find_package(SEQAN3 3.1)`.

# Note that seqan3-config.cmake can be standalone and thus SEQAN3_CLONE_DIR might be empty.
find_path (SEQAN3_CLONE_DIR NAMES build_system/seqan3-config.cmake HINTS "${CMAKE_CURRENT_LIST_DIR}/..")
find_path (SEQAN3_INCLUDE_DIR NAMES "seqan3/version.hpp" HINTS "${SEQAN3_CLONE_DIR}/include")

# extract version from seqan3/version.hpp header
file(STRINGS "${SEQAN3_INCLUDE_DIR}/seqan3/version.hpp" SEQAN3_VERSION_HPP REGEX "#define SEQAN3_VERSION_(MAJOR|MINOR|PATCH)")
string(REGEX REPLACE "#define SEQAN3_VERSION_(MAJOR|MINOR|PATCH) " "" PACKAGE_VERSION "${SEQAN3_VERSION_HPP}")
string(REGEX REPLACE ";" "." PACKAGE_VERSION "${PACKAGE_VERSION}")

if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)
  set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()

  if(PACKAGE_VERSION MATCHES "^([0-9]+)\\.")
    set(CVF_VERSION_MAJOR "${CMAKE_MATCH_1}")
  endif()

  if(PACKAGE_FIND_VERSION_MAJOR STREQUAL CVF_VERSION_MAJOR)
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
  else()
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
  endif()

  if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
      set(PACKAGE_VERSION_EXACT TRUE)
  endif()
endif()

# if the installed or the using project don't have CMAKE_SIZEOF_VOID_P set, ignore it:
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "")
  return()
endif()

# check that the installed version has the same 32/64bit-ness as the one which is currently searching:
if(NOT "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
  math(EXPR installedBits "8 * 8")
  set(PACKAGE_VERSION "${PACKAGE_VERSION} (${installedBits}bit)")
  set(PACKAGE_VERSION_UNSUITABLE TRUE)
endif()
