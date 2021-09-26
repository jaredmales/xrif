cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project("xrif")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c11") # -coverage") # -march=skylake")

find_package(OpenMP)
if(OPENMP_FOUND)
message("Found OpenMP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
pkg_check_modules(SUBUNIT libsubunit)
pkg_check_modules(CHECK check)
endif()

#######################################################################
#
#                            Source
#
#######################################################################
add_subdirectory(src)

#######################################################################
#
#                            Tests
#
#######################################################################
if(CHECK_FOUND AND SUBUNIT_FOUND)
add_subdirectory(tests)
enable_testing()
add_test(xrif_test_init tests/xrif_test_init)
add_test(xrif_test_difference_previous_whitenoise tests/xrif_test_difference_previous_whitenoise)
add_test(xrif_test_difference_first_whitenoise tests/xrif_test_difference_first_whitenoise)
add_test(xrif_test_difference_pixel_whitenoise tests/xrif_test_difference_pixel_whitenoise)
add_test(xrif_test_increment tests/xrif_test_increment)
add_test(xrif_test_whitenoise tests/xrif_test_whitenoise)
endif()

