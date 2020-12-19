
#######################################################################
#
#                            Tests
#
#######################################################################
add_executable(xrif_test_init xrif_test_init.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_init PUBLIC)

add_executable(xrif_test_whitenoise xrif_test_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_whitenoise PUBLIC)

add_executable(xrif_test_difference_previous_whitenoise xrif_test_difference_previous_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_difference_previous_whitenoise PUBLIC)

include_directories(${SUBUNIT_INCLUDE_DIRS})
link_directories(${SUBUNIT_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_whitenoise ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_difference_previous_whitenoise ${SUBUNIT_LIBRARIES})

include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_whitenoise ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_difference_previous_whitenoise ${CHECK_LIBRARIES})

if(LIBRT)
    target_link_libraries(xrif_test_init ${LIBRT})
    target_link_libraries(xrif_test_whitenoise ${LIBRT})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBRT})
endif()
if(LIBM)
    target_link_libraries(xrif_test_init ${LIBM})
    target_link_libraries(xrif_test_whitenoise ${LIBM})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBM})
endif()
if(LIBPTHREAD)
    target_link_libraries(xrif_test_init ${LIBPTHREAD})
    target_link_libraries(xrif_test_whitenoise ${LIBPTHREAD})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBPTHREAD})
endif()
