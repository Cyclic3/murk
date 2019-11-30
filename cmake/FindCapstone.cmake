find_path(CAPSTONE_INCLUDE_DIRS
  NAMES capstone/capstone.h
)

find_library(CAPSTONE_LIBRARIES NAMES capstone)
mark_as_advanced(CAPSTONE_LIBRARIES)
add_library(Capstone::capstone UNKNOWN IMPORTED)
set_target_properties(Capstone::capstone PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CAPSTONE_INCLUDE_DIRS}
  IMPORTED_LOCATION ${CAPSTONE_LIBRARIES}
)

find_package_handle_standard_args(Capstone DEFAULT_MSG
  CAPSTONE_LIBRARIES
)
