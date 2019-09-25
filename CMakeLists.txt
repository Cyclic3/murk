cmake_minimum_required(VERSION 3.10)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message("Using ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()


set(CMAKE_CXX_STANDARD 20)

project(murk)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cotire)

find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(TBB REQUIRED)
find_package(Botan REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cppthings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/span-lite)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)

file(GLOB_RECURSE murk_source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(murk ${murk_source})
cotire(murk)

target_link_libraries(murk
  PUBLIC
  Boost::boost
  Boost::system
  OpenSSL::SSL
  nonstd::span-lite
  Threads::Threads
  fmt::fmt-header-only
  TBB::tbb
  Botan::botan
  CppThings::cppthings
  PRIVATE
)

target_include_directories(murk
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

include(GNUInstallDirs REQUIRED)
install(TARGETS murk
    EXPORT Murk
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
set_target_properties(murk PROPERTIES EXPORT_NAME Murk)

enable_testing()
find_package(GTest REQUIRED)
file(GLOB_RECURSE tests_source test/*.cpp)
add_executable(tests ${tests_source})
target_link_libraries(tests murk GTest::GTest)
gtest_discover_tests(tests)

file(GLOB_RECURSE foo_source foo/*.cpp)
foreach(foo ${foo_source})
  file(RELATIVE_PATH foo_rel ${CMAKE_SOURCE_DIR}/foo ${foo})
  get_filename_component(foo_fname ${foo_rel} NAME_WE)
  get_filename_component(foo_dir ${foo_rel} DIRECTORY)
  string(REPLACE "/" "_" foo_dir ${foo_dir})

  set(foo_name ${foo_dir}_${foo_fname})

  add_executable(${foo_name} ${foo})

  target_link_libraries(${foo_name} murk)
endforeach()
