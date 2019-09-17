
#######################################################################
#
#                            Tests
#
#######################################################################
add_executable(xrif_test_init xrif_test_init.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_init PUBLIC)

include_directories(${SUBUNIT_INCLUDE_DIRS})
link_directories(${SUBUNIT_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${SUBUNIT_LIBRARIES})

include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${CHECK_LIBRARIES})

if(LIBRT)
    target_link_libraries(xrif_test_init ${LIBRT})
endif()
if(LIBM)
    target_link_libraries(xrif_test_init ${LIBM})
endif()
if(LIBPTHREAD)
    target_link_libraries(xrif_test_init ${LIBPTHREAD})
endif()
