# - Try to find RapidXML lib
#
# Once done this will define
#
#  RAPIDXML_FOUND - system has eigen lib with correct version
#  RAPIDXML_INCLUDE_DIR - the eigen include directory

# Breannan Smith (smith@cs.columbia.edu)

find_path(RAPIDXML_INCLUDE_DIR NAMES rapidxml.hpp
    PATHS
    ${RAPIDXML_ROOT}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_INSTALL_PREFIX}/include
    PATH_SUFFIXES rapidxml
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RapidXML DEFAULT_MSG RAPIDXML_INCLUDE_DIR)
mark_as_advanced(RAPIDXML_INCLUDE_DIR)

add_library(RapidXML::RapidXML INTERFACE IMPORTED)
target_include_directories(RapidXML::RapidXML INTERFACE "${RAPIDXML_INCLUDE_DIR}")
