find_path(Z3_INCLUDE_DIRS
  NAMES z3_version.h
)

find_library(Z3_LIBRARIES NAMES z3)
mark_as_advanced(Z3_LIBRARIES)
add_library(Z3::z3 UNKNOWN IMPORTED)
set_target_properties(Z3::z3 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${Z3_INCLUDE_DIRS}
  IMPORTED_LOCATION ${CAPSTONE_LIBRARIES}
)

find_package_handle_standard_args(Capstone DEFAULT_MSG
  CAPSTONE_LIBRARIES
)
