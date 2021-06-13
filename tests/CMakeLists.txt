
#######################################################################
#
#                            Tests
#
#######################################################################
add_executable(xrif_test_init xrif_test_init.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_init PUBLIC)

add_executable(xrif_test_increment xrif_test_increment.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_increment PUBLIC)

add_executable(xrif_test_whitenoise xrif_test_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_whitenoise PUBLIC)

add_executable(xrif_test_difference_previous_whitenoise xrif_test_difference_previous_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_difference_previous_whitenoise PUBLIC)

add_executable(xrif_test_difference_first_whitenoise xrif_test_difference_first_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_difference_first_whitenoise PUBLIC)

add_executable(xrif_test_difference_pixel_whitenoise xrif_test_difference_pixel_whitenoise.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_difference_pixel_whitenoise PUBLIC)

add_executable(xrif_test_ascii xrif_test_ascii.c $<TARGET_OBJECTS:objlib>)
target_compile_options(xrif_test_ascii PUBLIC)

include_directories(${SUBUNIT_INCLUDE_DIRS})
link_directories(${SUBUNIT_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_increment ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_whitenoise ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_difference_previous_whitenoise ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_difference_first_whitenoise ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_difference_pixel_whitenoise ${SUBUNIT_LIBRARIES})
target_link_libraries(xrif_test_ascii ${SUBUNIT_LIBRARIES})

include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})
target_link_libraries(xrif_test_init ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_increment ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_whitenoise ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_difference_previous_whitenoise ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_difference_first_whitenoise ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_difference_pixel_whitenoise ${CHECK_LIBRARIES})
target_link_libraries(xrif_test_ascii ${CHECK_LIBRARIES})

if(LIBRT)
    target_link_libraries(xrif_test_init ${LIBRT})
    target_link_libraries(xrif_test_increment ${LIBRT})
    target_link_libraries(xrif_test_whitenoise ${LIBRT})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBRT})
    target_link_libraries(xrif_test_difference_first_whitenoise ${LIBRT})
    target_link_libraries(xrif_test_difference_pixel_whitenoise ${LIBRT})
    target_link_libraries(xrif_test_ascii ${LIBRT})
endif()
if(LIBM)
    target_link_libraries(xrif_test_init ${LIBM})
    target_link_libraries(xrif_test_increment ${LIBM})
    target_link_libraries(xrif_test_whitenoise ${LIBM})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBM})
    target_link_libraries(xrif_test_difference_first_whitenoise ${LIBM})
    target_link_libraries(xrif_test_difference_pixel_whitenoise ${LIBM})
    target_link_libraries(xrif_test_ascii ${LIBM})
endif()
if(LIBPTHREAD)
    target_link_libraries(xrif_test_init ${LIBPTHREAD})
    target_link_libraries(xrif_test_increment ${LIBPTHREAD})
    target_link_libraries(xrif_test_whitenoise ${LIBPTHREAD})
    target_link_libraries(xrif_test_difference_previous_whitenoise ${LIBPTHREAD})
    target_link_libraries(xrif_test_difference_first_whitenoise ${LIBPTHREAD})
    target_link_libraries(xrif_test_difference_pixel_whitenoise ${LIBPTHREAD})
    target_link_libraries(xrif_test_ascii ${LIBPTHREAD})
endif()
