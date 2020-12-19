cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project("xrif")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c99")

find_package(OpenMP)
if(OPENMP_FOUND)
message("Found OpenMP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_library(LIBRT rt)
if(LIBRT_FOUND)
message("Explicitly requiring -lrt")
endif()
find_library(LIBM m)
if(LIBM_FOUND)
message("Explicitly requiring -lm")
endif()
find_library(LIBPTHREAD pthread)
if(LIBPTHREAD_FOUND)
message("Explicitly requiring -lpthread")
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
add_test(xrif_test_whitenoise tests/xrif_test_whitenoise)
endif()

