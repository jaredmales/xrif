

# list of source files
set(libsrc xrif.c xrif_difference_previous.c xrif_difference_first.c xrif_difference_pixel.c lz4/lz4.c lz4/lz4hc.c )

# this is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})

# shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(xrif-shared SHARED $<TARGET_OBJECTS:objlib>)
add_library(xrif-static STATIC $<TARGET_OBJECTS:objlib>)

#This makes it so we have libxrif.so and libxrif.a, without the -shared/-static
SET_TARGET_PROPERTIES(xrif-shared PROPERTIES OUTPUT_NAME xrif CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(xrif-static PROPERTIES OUTPUT_NAME xrif CLEAN_DIRECT_OUTPUT 1)


#######################################################################
#
#                            Installation
#
#######################################################################

install (TARGETS xrif-shared DESTINATION lib)
install (TARGETS xrif-static DESTINATION lib)
install (FILES xrif.h DESTINATION include/xrif)
install (FILES lz4/lz4.h DESTINATION include/xrif/lz4)
install (FILES lz4/lz4hc.h DESTINATION include/xrif/lz4)

