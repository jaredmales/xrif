cmake_minimum_required(VERSION 3.10)

project("xrif")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fopenmp")

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
add_subdirectory(tests)
enable_testing() 
add_test(xrif_test_init tests/xrif_test_init) 



