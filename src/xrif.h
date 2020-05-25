/** \file xrif.h
  * \brief The eXtreme-ao Reordered Image Format: Declarations
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2019, 2020, The Arizona Board of Regents on behalf of The
University of Arizona

All rights reserved.

Developed by: The Arizona Board of Regents on behalf of the
University of Arizona.

Redistribution and use for noncommercial purposes in source and
binary forms, with or without modification, are permitted provided
that the following conditions are met:

1. The software is used solely for noncommercial purposes.

2. Redistributions of source code must retain the above copyright
notice, terms and conditions specified herein and the disclaimer
specified in Section 4 below.

3. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

4. Neither the name of the Arizona Board of Regents, the University
of Arizona nor the names of other contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

Arizona Required Clauses:

1.1. Arbitration. The parties agree that if a dispute arises
between them concerning this Agreement, the parties may be required
to submit the matter to arbitration pursuant to Arizona law.

1.2. Applicable Law and Venue. This Agreement shall be interpreted
pursuant to the laws of the State of Arizona. Any arbitration or
litigation between the Parties shall be conducted in Pima County,
ARIZONA, and LICENSEE hereby submits to venue and jurisdiction in
Pima County, ARIZONA.

1.3. Non-Discrimination. The Parties agree to be bound by state and
federal laws and regulations governing equal opportunity and non-
discrimination and immigration.

1.4. Appropriation of Funds. The Parties recognize that performance
by ARIZONA may depend upon appropriation of funds by the State
Legislature of ARIZONA. If the Legislature fails to appropriate the
necessary funds, or if ARIZONA’S appropriation is reduced during
the fiscal year, ARIZONA may cancel this Agreement without further
duty or obligation. ARIZONA will notify LICENSEE as soon as
reasonably possible after it knows of the loss of funds.

1.5. Conflict of Interest. This Agreement is subject to the
provisions of A.R.S. 38-511 and other conflict of interest
regulations. Within three years of the EFFECTIVE DATE, ARIZONA may
cancel this Agreement if any person significantly involved in
initiating, negotiating, drafting, securing, or creating this
Agreement for or on behalf of ARIZONA becomes an employee or
consultant in any capacity of LICENSEE with respect to the subject
matter of this Agreement.

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
   
#include <stdio.h> //just for debug
   
   
//#include <complex.h>

#include "lz4/lz4.h"
#include "lz4/lz4hc.h"





#define XRIF_VERSION (0)
#define XRIF_HEADER_SIZE (48)

#define XRIF_DIFFERENCE_NONE (-1)
#define XRIF_DIFFERENCE_DEFAULT (100)
#define XRIF_DIFFERENCE_PREVIOUS (100)
#define XRIF_DIFFERENCE_FIRST (200)
#define XRIF_DIFFERENCE_PIXEL (300)

#define XRIF_REORDER_NONE (-1)
#define XRIF_REORDER_DEFAULT (100)
#define XRIF_REORDER_BYTEPACK (100)
#define XRIF_REORDER_BYTEPACK_RENIBBLE (200)
#ifdef XRIF_EXPERIMENTAL
   //bitpack does not currently work.
   #define XRIF_REORDER_BITPACK (300)
#endif
   
#define XRIF_COMPRESS_NONE (-1)
#define XRIF_COMPRESS_DEFAULT (100)
#define XRIF_COMPRESS_LZ4 (100)

/// The type used for storing the width and height and depth dimensions of images.
typedef uint32_t xrif_dimension_t;

/** \defgroup error_codes Error Codes
  * \brief Error codes used by the xrif library
  * 
  * This library defines an error code type (merely an int) and a number of codes, all less than zero, to report errors.
  * In general we avoid in-band error reporting, with a few exceptions (e.g. xrif_typesize).
  *
  * @{
  */

/// The error reporting type.  
typedef int xrif_error_t;

#define XRIF_NOERROR (0)
#define XRIF_ERROR_NULLPTR (-5)
#define XRIF_ERROR_NOT_SETUP    (-10)
#define XRIF_ERROR_INVALID_SIZE (-20)
#define XRIF_ERROR_INVALID_TYPE (-22)
#define XRIF_ERROR_INSUFFICIENT_SIZE (-25)
#define XRIF_ERROR_MALLOC (-30)
#define XRIF_ERROR_NOTIMPL (-100)
#define XRIF_ERROR_BADHEADER (-1000);
#define XRIF_ERROR_WRONGVERSION (-1010);

///@}

/** \defgroup typecodes Type Codes 
  * \brief Type codes for storing the type of the data.  
  * 
  * These are identical to the ImageStreamIO data types.
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


/** \defgroup xrif_interface The main xrif library interface 
  * @{
  */

/// The xrif library configuration structure, organizing various parameters used by the functions.
/** This structure provides for setup and management of memory allocation, though externally allocated
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
                                    
                  
   unsigned char calc_performance; ///< Flag (true/false) controlling whether performance metrics are calculated.  Default is true.
   
   double compression_ratio;
   double encode_time;
   double encode_rate;
   double difference_time;
   double difference_rate;
   double reorder_time;
   double reorder_rate;
   double compress_time;
   double compress_rate;
   
   struct timespec ts_difference_start;
   struct timespec ts_reorder_start;
   struct timespec ts_compress_start;
   struct timespec ts_compress_done;
   
   struct timespec ts_decompress_start;
   struct timespec ts_unreorder_start;
   struct timespec ts_undifference_start;
   struct timespec ts_undifference_done;
   
} xrif_handle;

/// The xrif handle pointer type.  This provides the main interface to the xrif library.
typedef xrif_handle* xrif_t;

/// Allocate a handle and initialize it.
/** The argument is a pointer to `xrif_t`, making it the address of an `xrif_handle` pointer.
  * 
  * \returns XRIF_ERROR_NULLPTR if a null pointer is passed
  * \returns XRIF_ERROR_MALLOC on an allocation error
  * \returns XRIF_NOERROR on success
  */ 
xrif_error_t xrif_new(xrif_t * handle_ptr /**< [out] a pointer to an xrif handle. */);

/// Deallocate a handle, including any memory that it owns.
/**
  * \returns XRIF_ERROR_NULLPTR if a null pointer is passed
  * \returns XRIF_NOERROR on success
  */ 
xrif_error_t xrif_delete(xrif_t handle /**< [in] an xrif handle which has been initialized with xrif_new */);

///@}

/** \defgroup xrif_init Initialization, Setup, and Allocation 
  * \ingroup xrif_interface
  * @{
  */

/// Initialize an xrif handle object.
/** Sets all values to defaults, and ensures that 
  * calls to allocate functions or xrif_destroy_handle
  * will safely succeed.
  *
  * In general this should not be called independently, rather you should use
  * xrif_new.  If you do, this function must only be called on an xrif handle which does
  * not already have memory alocated -- otherwise memory leaks will occur! 
  * 
  * \returns XRIF_ERROR_NULLPTR if handle is NULL.
  * \returns XRIF_NOERROR on success
  */ 
xrif_error_t xrif_initialize_handle( xrif_t handle /**< [out] the xrif handle to initialize */);

/// Set the basic parameters of an xrif handle
/** After setting these parameters, a call to one of the allocate or set functions
  * will succceed.
  *
  * \returns XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns XRIF_ERROR_INVALID_SIZE if any of `w`, `h`, `d`, or `f` are 0.
  * \returns XRIF_ERROR_INVALID_TYPE if `c` specifies an invalid type code
  * \returns XRIF_NOERROR on success
  */
xrif_error_t xrif_set_size( xrif_t handle,      ///< [in/out] the xrif handle to be set up
                            xrif_dimension_t w, ///< [in] the width of a single frame of data, in pixels
                            xrif_dimension_t h, ///< [in] the height of a single frame of data, in pixels
                            xrif_dimension_t d, ///< [in] the depth of a single frame of data, in pixels
                            xrif_dimension_t f, ///< [in] the number of frames of data, each frame having w X h x d pixels
                            xrif_typecode_t c   ///< [in] the code specifying the data type
                          );

/// Set the raw data buffer to a pre-allocated pointer
/** You are responsible for allocating the buffer to be > width*height*frames*size().
  * This will return an error if size is too small for the currently set values.
  * 
  * This pointer will not be free()-ed on a call to xrif_destroy_handle.
  *
  * \returns XRIF_ERROR_INVALID_SIZE if bad values are passed for raw or size
  * \returns XRIF_NOERROR on success
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
/** You must call \ref xrif_set_size prior to calling this function.
  * 
  * \returns XRIF_ERROR_NOT_SETUP if the handle has not been configured
  * \returns XRIF_ERROR_MALLOC on an error from `malloc`
  * \returns XRIF_NOERROR on success. 
  */ 
xrif_error_t xrif_allocate( xrif_t handle   /**< [in/out] the xrif object to be allocated */);


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
  * \ingroup xrif_interface
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

/** \defgroup xrif_encode Encoding & Decoding
  * \ingroup xrif_interface
  * 
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

/** \defgroup xrif_diff Differencing
  * \ingroup xrif_encode
  * 
  * @{
  */

xrif_error_t xrif_difference( xrif_t handle );

xrif_error_t xrif_undifference( xrif_t handle );

/// Difference all images with respect to previous image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_previous_sint16( xrif_t handle /**< [in/out] the xrif handle */);

/// Difference all images with respect to previous image, for uint64_t data type
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_previous_uint64( xrif_t handle /**< [in/out] the xrif handle */);

/// Difference all images with respect to the first image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_first_sint16( xrif_t handle/**< [in/out] the xrif handle */ );

/// Difference all images with respect to the first image, for uint64_t data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_first_uint64( xrif_t handle/**< [in/out] the xrif handle */ );

xrif_error_t xrif_difference_sint16_rgb( xrif_t handle /**< [in/out] the xrif handle */);

/// Difference each image with respect to its first pixel, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_difference_pixel_sint16( xrif_t handle /**< [in/out] the xrif handle */);

/// Reverse the differencing operation, w.r.t. the previous image, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_undifference_previous_sint16( xrif_t handle /**< [in/out] the xrif handle */);

///@}

/// Reverse the differencing operation on each image with respect to its first pixel, for short integer data type.
/**
  *
  * \returns 0 on success.
  * \returns <0 on error, with the appropreate error code.
  */ 
xrif_error_t xrif_undifference_pixel_sint16( xrif_t handle /**< [in/out] the xrif handle */);


/** \defgroup xrif_reorder Reordering
  * \ingroup xrif_encode 
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

/** \defgroup xrif_compress Compression 
  * \ingroup xrif_encode
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


/** \defgroup xrif_performance Performance Measurements
  * \ingroup xrif_interface
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


/** \defgroup xrif_utils Utilities
  *
  * @{
  */ 

/// Return the size of the type specified by the code.
/**
  * \returns the equivalent to `sizeof(type)` for the specified type code.
  * \returns 0 if the `type_code` is invalid
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

/// Get a string describing the difference method
/** Returns a pointer to a string describing the difference method.
  *
  * \returns a pointer to the string if `diff_method` is valid
  * \returns NULL if `diff_method` is not valid
  */  
const char * xrif_difference_method_string( int diff_method /**< [in] the difference method */);

/// Get a string describing the reordering method
/** Returns a pointer to a string describing the reordering method.
  *
  * \returns a pointer to the string if `reorder_method` is valid
  * \returns NULL if `reorder_method` is not valid
  */  
const char * xrif_reorder_method_string( int reorder_method /**< [in] the reorder method*/);

/// Get a string describing the compression method
/** Returns a pointer to a string describing the compression method.
  *
  * \returns a pointer to the string if `compress_method` is valid
  * \returns NULL if `compress_method` is not valid
  */  
const char * xrif_compress_method_string( int compress_method /**< [in] the compression method*/);

///@}

#ifdef __cplusplus
//extern "C"
}
#endif
   
#endif //xrif_xrif_h
