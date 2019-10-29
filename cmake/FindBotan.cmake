find_path(BOTAN_INCLUDE_DIRS
  PATH_SUFFIXES botan-2
  NAMES botan/botan.h
)

find_library(BOTAN_LIBRARIES NAMES botan-2)
mark_as_advanced(BOTAN_LIBRARIES)
add_library(Botan::botan UNKNOWN IMPORTED)
set_target_properties(Botan::botan PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${BOTAN_INCLUDE_DIRS}
  IMPORTED_LOCATION ${BOTAN_LIBRARIES}
)

find_package_handle_standard_args(Botan DEFAULT_MSG
  BOTAN_INCLUDE_DIRS BOTAN_LIBRARIES
)
