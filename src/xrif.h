/** \file xrif.h
  * \brief The eXtreme-ao Reordered Image Format: Declarations
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

#ifndef xrif_xrif_h
#define xrif_xrif_h

#ifdef __cplusplus
extern "C"
{
#endif

   
#ifndef LZ4_MEMORY_USAGE
#define LZ4_MEMORY_USAGE (20)
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <complex.h>

#include "lz4/lz4.h"
#include "lz4/lz4hc.h"





#define XRIF_VERSION (0)
#define XRIF_HEADER_SIZE (48)

#define XRIF_DIFFERENCE_NONE (-1)
#define XRIF_DIFFERENCE_DEFAULT (100)
#define XRIF_DIFFERENCE_PREVIOUS (100)
#define XRIF_DIFFERENCE_FIRST (200)

#define XRIF_REORDER_NONE (-1)
#define XRIF_REORDER_DEFAULT (200)
#define XRIF_REORDER_BYTEPACK (100)
#define XRIF_REORDER_BYTEPACK_RENIBBLE (200)
#define XRIF_REORDER_BITPACK (300)

#define XRIF_COMPRESS_NONE (-1)
#define XRIF_COMPRESS_DEFAULT (100)
#define XRIF_COMPRESS_LZ4 (100)

/// The type used for storing the width and height and depth dimensions of images.
typedef uint32_t xrif_dimension_t;

typedef int xrif_error_t;

#define XRIF_NOERROR (0)
#define XRIF_ERROR_NULLPTR (-5)
#define XRIF_ERROR_NOT_SETUP    (-10)
#define XRIF_ERROR_INVALID_SIZE (-20)
#define XRIF_ERROR_INSUFFICIENT_SIZE (-25)
#define XRIF_ERROR_MALLOC (-30)
#define XRIF_ERROR_NOTIMPL (-100)
#define XRIF_ERROR_BADHEADER (-1000);
#define XRIF_ERROR_WRONGVERSION (-1010);


/** \name Typecodes for storing the type of the data.  These are identical to the ImageStreamIO data types.
  * @{
  */

/// The type used for storing the ImageStreamIO data type code.
typedef uint8_t xrif_typecode_t;

#define XRIF_TYPECODE_UINT8          (1)  /**< 8-bit unsigned integer */  
#define XRIF_TYPECODE_INT8           (2)  /**< 8-bit signed integer */
#define XRIF_TYPECODE_UINT16         (3)  /**< 16-bit unsigned integer */
#define XRIF_TYPECODE_INT16          (4)  /**< 16-bit signed integer */
#define XRIF_TYPECODE_UINT32         (5)  /**< 32-bit unsigned integer */
#define XRIF_TYPECODE_INT32          (6)  /**< 32-bit signed integer */
#define XRIF_TYPECODE_UINT64         (7)  /**< 64-bit unsigned integer */
#define XRIF_TYPECODE_INT64          (8)  /**< 64-bit signed integer */
#define XRIF_TYPECODE_HALF           (13) /**< IEE 754 half-precision 16-bit (uses uint16_t for storage) */
#define XRIF_TYPECODE_FLOAT          (9)  /**< IEEE 754 single-precision binary floating-point format: binary32 */
#define XRIF_TYPECODE_DOUBLE         (10) /**< IEEE 754 double-precision binary floating-point format: binary64 */
#define XRIF_TYPECODE_COMPLEX_FLOAT  (11) /**< complex float  */
#define XRIF_TYPECODE_COMPLEX_DOUBLE (12) /**< complex double */

/// @}

   
/// The xrif library configuration structure, organizing various parameters used by the functions.
/** This structure provides setup and management of memory allocation, though externally allocated
  * buffers can be used when desired.
  *
  * Options related to compression level and speed are also provided.
  * 
  * It is intended that this structure be interacted with via the xrif_t typedef, which is a pointer
  * to xrif_handle. Values of this structure should generally be changed by one of the xrif_set_*() functions,
  * to allow for error checking and consistency.  Unless you know what you're doing, of course.
  * 
  */ 
typedef struct
{
   xrif_dimension_t width;     ///< The width of a single image, in pixels
   xrif_dimension_t height;    ///< The height of a single image, in pixels
   xrif_dimension_t depth;     ///< The depth of a single image, in pixels
   xrif_dimension_t frames;    ///< The number of frames in the stream
   
   xrif_typecode_t type_code;  ///< The code specifying the data type of the pixels
   
   size_t data_size;     ///< The size of the pixels, bytes.

   size_t raw_size;         ///< Size of the stream before compression
   size_t compressed_size;  ///< Size of the stream after compression
   
   int difference_method; ///< The difference method to use.
   
   int reorder_method;   ///< The method to use for bit reordering.
   
   int compress_method; ///< The compression method used.
   
   int lz4_acceleration; ///< LZ4 acceleration parameter, >=1, higher is faster with less comporession.  Default is 1.
   
   int omp_parallel;     /**< Flag controlling whether OMP parallelization is used to speed up.  This has no effect if XRIF_NO_OMP is defined at compile time, 
                              which completely removes OMP code. Default is 0.*/
   
   int omp_numthreads;   /**< Number of threads to use if omp_parallel is 1.  For this to be meaninfgul, 
                           *  XRIF_NO_OMP must NOT be defined at compile time, and XRIF_OMP_NUMTHREADS must be defined at compile time. Default is 1.*/
   
   unsigned char compress_on_raw; ///< Flag (true/false) indicating whether the raw buffer is used for compression.  Default on initializeation is true.
   
   unsigned char own_raw;  ///< Flag (true/false) indicating whether the raw_buffer pointer is managed by this handle
   char * raw_buffer;      ///< The raw buffer pointer, contains the image data, both before and after compression
   size_t raw_buffer_size; /**< The size of the raw_buffer pointer.  If `compress_on_raw` is false, then this must be at least width*height*depth*frames*data_size. If
                             *` compress_on_raw` is true, this should be at least LZ4_compressBound(width*height*depth*frames*data_size) in size, but this is not a strict 
                             *  requirement in practice for most streams.  If this library is used to allocate it, it will be the larger of the two possibilities.*/
   
   unsigned char own_reordered;   ///< Flag (true/false) indicating whether the reordered_buffer pointer is managed by this handle.
   char * reordered_buffer;       ///< The reordered buffer pointer, contains the reordered data.
   size_t reordered_buffer_size;  ///< The size of the reordered_buffer pointer.  It must be at least width*height*depth*frames*data_size.
   
   unsigned char own_compressed;  ///< Flag (true/false) indicating whether the compressed_buffer pointer is managed by this handle.
   char * compressed_buffer;      ///< The compressed buffer pointer, contains the compressed data.
   size_t compressed_buffer_size; /**< The size of the compressed_buffer pointer.  In principle should be at least LZ4_compressBound(width*height*depth*frames*data_size) 
                                    *  in size, but this is not a strict requirement in practice for most streams.  It must be at least width*height*depth*frames*data_size.  
                                    *  If this library is used to allocate it, it will be the larger of the two.*/
                                    
                                    
   struct timespec ts_difference_start;
   struct timespec ts_reorder_start;
   struct timespec ts_compress_start;
   struct timespec ts_compress_done;
   
   struct timespec ts_decompress_start;
   struct timespec ts_unreorder_start;
   struct timespec ts_undifference_start;
   struct timespec ts_undifference_done;
   
} xrif_handle;

/// The xrif handle pointer type.  This is the main interface to the xrif library.
typedef xrif_handle* xrif_t;

/// Allocate a handle and initialize it.
/**
  *
  * \returns
  */ 
xrif_error_t xrif_new(xrif_t * handle_ptr /**< [out] a pointer to an xrif handle. */);

/// Deallocate a handle, including any memory that it owns.
/**
  *
  * \returns
  */ 
xrif_error_t xrif_delete(xrif_t handle /**< [in] an xrif handle which has been initialized with xrif_malloc */);


/** \name Initialization, Setup, and Allocation 
  *
  * @{
  */

/// Initialize an xrif handle object.
/** Sets all values to defaults, and ensures that 
  * calls to allocate functions or xrif_destroy_handle
  * will safely succeed.
  *
  * This function must only be called on an xrif handle which does
  * not already have memory alocated -- otherwise memory leaks will occur!
  * 
  * \returns 0 on success
  * \returns < 0 on error, with an appropreate xrif error code.
  */ 
xrif_error_t xrif_initialize_handle( xrif_t handle /**< [out] the xrif handle to initialize */);

/// Set the basic parameters of an xrif handle
/** After setting these parameters, a call to one of the allocate or set functions
  * will succceed.
  *
  * \returns XRIF_NOERROR on success
  * \returns < 0 on error, with an appropriate xrif error code. [no errors currently implemented]
  */
xrif_error_t xrif_set_size( xrif_t handle,  ///< [in/out] the xrif handle to be set up
                            xrif_dimension_t w,     ///< [in] the width of a single frame of data, in pixels
                            xrif_dimension_t h,     ///< [in] the height of a single frame of data, in pixels
                            xrif_dimension_t d,     ///< [in] the depth of a single frame of data, in pixels
                            xrif_dimension_t f,     ///< [in] the number of frames of data, each frame having w X h x d pixels
                            xrif_typecode_t c ///< [in] the code specifying the data type
                          );

/// Set the raw data buffer to a pre-allocated pointer
/** You are responsible for allocating the buffer to be > width*height*frames*size().
  * This will return an error if size is too small for the currently set values.
  * 
  * This pointer will not be free()-ed on a call to xrif_destroy_handle.
  *
  * \returns XRIF_NOERROR on success
  * \returns XRIF_ERROR_INVALID_SIZE if bad values are passed for raw or size
  */  
xrif_error_t xrif_set_raw( xrif_t handle,  ///< [in/out] the xrif handle
                           void * raw,       ///< [in] the pointer to a pre-allocated block
                           size_t size       ///< [in] the size of the pre-allocated block
                         );

/// Allocate the raw buffer based on the already set stream dimensions.
/** If the raw_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * The size will be set to the maximum of the pre-setup data size and the 
  * LZ4_compressBound result.  LZ4 typically (in all tested cases) requests
  * a few hundred more bytes for compression.
  * 
  * \returns 0 on success
  * \returns -1 on error, and handle->error will be set.
  */
xrif_error_t xrif_allocate_raw( xrif_t handle /**< [in/out] the xrif object to modify */);


/// Set the rordered (working) data buffer to a pre-allocated pointer
/** You are responsible for allocating the buffer to be > width*height*frames*size().
  * 
  * 
  * This pointer will not be free()-ed on a call to xrif_destroy_handle.
  *
  * \returns 0 on success
  * \returns < 0 on error, with the appropriate XRIF_ERROR_* code.
  */  
xrif_error_t xrif_set_reordered( xrif_t handle,  ///< [in/out] the xrif object to modify
                                 void * reordered, ///< [in] pointer to a pre-allocated block
                                 size_t size       ///< [in] the size of the pre-allocated block
                               );

/// Allocate the reordered buffer based on the already set stream dimensions.
/** If the reordered_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * \returns 0 on success
  * \returns -1 on error, and handle->error will be set.
  */
xrif_error_t xrif_allocate_reordered( xrif_t handle /**< [in/out] the xrif object to modify */);

/// Set the compressed data buffer to a pre-allocated pointer
/** You are responsible for allocating the buffer to be > width*height*frames*size().
  * 
  * 
  * This pointer will not be free()-ed on a call to xrif_destroy_handle.
  *
  * \returns 0 on success
  * \returns < 0 on error, with the appropriate XRIF_ERROR_* code.
  */  
xrif_error_t xrif_set_compressed( xrif_t handle,  ///< [in/out] the xrif object to modify
                                  void * reordered, ///< [in] pointer to a pre-allocated block
                                  size_t size       ///< [in] the size of the pre-allocated block
                                );

/// Allocate the compressed buffer based on the already set stream dimensions.
/** If the compressed_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * \returns 0 on success
  * \returns -1 on error, and handle->error will be set.
  */
xrif_error_t xrif_allocate_compressed( xrif_t handle /**< [in/out] the xrif handle */);


/// Allocate all memory buffers according to the configuration specified in the handle.
/**
  */ 
xrif_error_t xrif_allocate( xrif_t handle,  ///< [in/out] the xrif object to be allocated
                            xrif_dimension_t w,
                            xrif_dimension_t h,
                            xrif_dimension_t d,
                            xrif_dimension_t f,
                            xrif_typecode_t c
                          );

/// Destroy a handle, de-allocating owned pointers and re-initializing
/** Free()s the raw and reordered buffers (if owned by this handle) and 
  * calls xrif_initialize().
  *
  * \returns 0 on success
  * \returns <0 on error, with the appropriate code
  */
xrif_error_t xrif_destroy_handle( xrif_t handle /**< [in/out] the xrif handle */);

/// @}

/** \name Header Processing 
  *
  * These functions populate or read the xrif header.
  *
  * @{
  */

xrif_error_t xrif_write_header( char * header,
                                xrif_t handle 
                              );

xrif_error_t xrif_read_header( xrif_t handle,
                               uint32_t * header_size,
                               char * header
                             );

///@}
/** \name Encoding & Decoding
  * These functions perform a complete cycle of encode and decode.
  *
  * @{
  */

/// Encode data using the xrif format 
/** Calls xrif_difference, xrif_reorder, and xrif_compress.
  * If any of these returns an error, that error is returned.
  *
  * The timespecs are updated during this call.
  * 
  * \returns XRIF_NOERROR on success
  * \returns an xrif error code otherwise.
  * 
  */
xrif_error_t xrif_encode( xrif_t handle /**< [in/out] the xrif handle */);

/// Decode data from the xrif format 
/** Calls xrif_decompress, xrif_unreorder, and xrif_undifference.
  * If any of these returns an error, that error is returned.
  * 
  * The timespecs are updated during this call.
  * 
  * \returns XRIF_NOERROR on success
  * \returns an xrif error code otherwise.
  * 
  */
xrif_error_t xrif_decode( xrif_t handle /**< [in/out] the xrif handle */);


/// @}

/** \name Differencing
  *
  * @{
  */

xrif_error_t xrif_difference( xrif_t handle );

xrif_error_t xrif_undifference( xrif_t handle );

/// Difference all images writh respect to previous image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_previous_sint16( xrif_t handle /**< [in/out] the xrif handle */);

/// Difference all images writh respect to the first image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_first_sint16( xrif_t handle/**< [in/out] the xrif handle */ );


xrif_error_t xrif_difference_sint16_rgb( xrif_t handle /**< [in/out] the xrif handle */);

/// Reverse the differencing operation, w.r.t. the previous image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_undifference_previous_sint16( xrif_t handle /**< [in/out] the xrif handle */);

///@}

/** \name Reordering
  * @{
  */

/// Reorder the data using the method specified by `reorder_method`
/**
  * \returns XRIF_ERROR_NONE on success
  * \returns an error code on an error.
  */ 
xrif_error_t xrif_reorder( xrif_t handle /**< [in/out] the xrif handle */);

/// Un-reorder the data using the method specified by `reorder_method`
/**
  * \returns XRIF_ERROR_NONE on success
  * \returns an error code on an error.
  */ 
xrif_error_t xrif_unreorder( xrif_t handle /**< [in/out] the xrif handle */);

/// Perform no re-ordering, simply copy raw to reordered.
/** Primarilly for testing and benchmarking.
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropriate error code.
  */ 
xrif_error_t xrif_reorder_none( xrif_t handle /**< [in/out] the xrif handle */ );

xrif_error_t xrif_reorder_bytepack( xrif_t handle /**< [in/out] the xrif handle */ );

xrif_error_t xrif_reorder_bytepack_sxrif_error_t16( xrif_t handle /**< [in/out] the xrif handle */ );

xrif_error_t xrif_reorder_bytepack_renibble( xrif_t handle /**< [in/out] the xrif handle */ );

xrif_error_t xrif_reorder_bytepack_renibble_short( xrif_t handle /**< [in/out] the xrif handle */ );

xrif_error_t xrif_reorder_bitpack( xrif_t handle /**< [in/out] the xrif handle */ );
      
xrif_error_t xrif_reorder_bitpack_sint16( xrif_t handle /**< [in/out] the xrif handle */ );

         
xrif_error_t xrif_unreorder_none( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_unreorder_bytepack( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_unreorder_bytepack_renibble( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_unreorder_bitpack( xrif_t handle /**< [in/out] the xrif handle */);

///@}

/** \name Compression 
  *
  * @{
  */ 
xrif_error_t xrif_compress( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_decompress(xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_compress_none( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_decompress_none( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_compress_lz4( xrif_t handle /**< [in/out] the xrif handle */);

xrif_error_t xrif_decompress_lz4( xrif_t handle /**< [in/out] the xrif handle */);

///@}


/** \name Performance
  *
  * @{
  */ 

/// Calculate the compression ratio 
/** 
  * \returns the ratio of compressed_size to raw_size.
  */ 
double xrif_compression_ratio( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to encode the data
/**
  * \returns the difference in the timespecs marking the beginning of differencing the end of compression 
  */ 
double xrif_encode_time ( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the encode rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_encode_time
  */ 
double xrif_encode_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to difference the data
/**
  * \returns the difference in the timespecs marking the beginning of reordering the beginning of differencing 
  */ 
double xrif_difference_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the differencing rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_difference_time
  */ 
double xrif_difference_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to reorder the data
/**
  * \returns the difference in the timespecs marking the beginning of compression and the beginning of reordering
  */ 
double xrif_reorder_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the reordering rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_reorder_time
  */ 
double xrif_reorder_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to compress the differenced and reordered data
/**
  * \returns the difference in the timespecs marking the beginning of reordering and the end of compression
  */ 
double xrif_compress_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the compression rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_compress_time
  */ 
double xrif_compress_rate( xrif_t handle /**< [in/out] the xrif handle */);



///@}


/** \name Utilities
  *
  * @{
  */ 

/// Return the size of the type specified by the code.
/**
  * \returns the equivalent to `sizeof(type)` for the specified type code.
  */  
size_t xrif_typesize( xrif_typecode_t type_code /**< [in] the type code*/);

/// Calculate the difference between two timespecs.
/** Calculates `ts1-ts0` in `double` precision.
  * 
  * \returns the difference in seconds between the two timespecs 
  */
double xrif_ts_difference( struct timespec * ts1, ///< [in] the end time
                           struct timespec * ts0  ///< [in] the start time
                         );

///@}

#ifdef __cplusplus
//extern "C"
}
#endif
   
#endif //xrif_xrif_h
