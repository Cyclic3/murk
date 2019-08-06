cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(murk)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(TBB REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)

file(GLOB_RECURSE murk_source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(murk ${murk_source})

target_link_libraries(murk
  PUBLIC
  Boost::boost
  Boost::system
  OpenSSL::SSL
  gsl::gsl-lite
  Threads::Threads
  fmt::fmt-header-only
  TBB::tbb
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
add_executable(foo ${foo_source})
target_link_libraries(foo murk)
