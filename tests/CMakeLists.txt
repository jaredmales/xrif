

#######################################################################
#
#                            Tests
#
#######################################################################
add_executable(xrif_test_init xrif_test_init.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_init PUBLIC -fopenmp)
target_link_libraries(xrif_test_init check subunit pthread m rt )