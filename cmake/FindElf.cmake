find_path(ELF_INCLUDE_DIRS
  NAMES libelf.h
)

find_library(ELF_LIBRARIES NAMES elf)
mark_as_advanced(ELF_LIBRARIES)
add_library(Elf::elf UNKNOWN IMPORTED)
set_target_properties(Elf::elf PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${ELF_INCLUDE_DIRS}
  IMPORTED_LOCATION ${ELF_LIBRARIES}
)

find_package_handle_standard_args(Elf DEFAULT_MSG
  ELF_LIBRARIES
)
