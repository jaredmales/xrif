/** \file xrif.h
  * \brief The eXtreme-ao Reordered Image Format: Declarations
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2019, 2020, 2021, The Arizona Board of Regents on behalf of The
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

//Needed for CLOCK_REALTIME:
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

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
#include "fastlz/fastlz.h"
#include <zlib.h>
#include <zstd.h>


#define XRIF_VERSION (0)
#define XRIF_HEADER_SIZE (48)


/// The error reporting type. 
/** \ingroup xrif_types 
  */
typedef int xrif_error_t;

/// The type used for general integers.
/** \ingroup xrif_types 
  */
typedef int xrif_int_t;

/// The type used for storing the width and height and depth dimensions of images.
/** \ingroup xrif_types 
  */
typedef uint32_t xrif_dimension_t;



/// No differencing
/** \ingroup diff_methods
  */
#define XRIF_DIFFERENCE_NONE (-1)

/// Previous differencing, where the previous frame is used as the reference
/** \ingroup diff_methods
  */
#define XRIF_DIFFERENCE_PREVIOUS (100)

/// First differencing, where the first frame is used as a the reference
/** \ingroup diff_methods
  */
#define XRIF_DIFFERENCE_FIRST (200)

/// Pixel differencing, where the previous pixel is used as the reference
/** \ingroup diff_methods
  */
#define XRIF_DIFFERENCE_PIXEL (300)

/// Default differencing, identical to PREVIOUS
/** \ingroup diff_methods
  */
#define XRIF_DIFFERENCE_DEFAULT (100)



/// No reordering
/** \ingroup reorder_methods
  */
#define XRIF_REORDER_NONE (-1)

/// Bytepack reodering
/** \ingroup reorder_methods
  */
#define XRIF_REORDER_BYTEPACK (100)

/// Bytepack reodering with renibble.
/** \ingroup reorder_methods
  */
#define XRIF_REORDER_BYTEPACK_RENIBBLE (200)

/// Bitpack reoderering.
/** \ingroup reorder_methods
  */
#define XRIF_REORDER_BITPACK (300)

/// Default reordering, equivalent to bytepack
/** \ingroup reorder_methods
  */
#define XRIF_REORDER_DEFAULT (100)



/// Use no compression
/** \ingroup compress_methods
  */
#define XRIF_COMPRESS_NONE (-1)

/// Use the default compression method which is LZ4
/** \ingroup compress_methods
  */
 #define XRIF_COMPRESS_DEFAULT (100)

/// Use LZ4 for compression
/** See \ref lz4_config for options.
  *
  * \ingroup compress_methods
  */
#define XRIF_COMPRESS_LZ4 (100)

/// Use LZ4HC for compression
/** See \ref lz4hc_config for options.
  * 
  * \ingroup compress_methods
  */
#define XRIF_COMPRESS_LZ4HC (102)

/// Use FastLZ for compression
/** See \ref fastlz_config for options.
  * 
  * \ingroup compress_methods
  */
#define XRIF_COMPRESS_FASTLZ (110)

/// Use zstd for compression
/** See \ref zstd_config for options.
  * 
  * \ingroup compress_methods
  */
#define XRIF_COMPRESS_ZSTD (120)

/// Use zlib for compression
/** See \ref zlib_config for options.
  * 
  * \ingroup compress_methods
  */
#define XRIF_COMPRESS_ZLIB (130)



/// The minimum possible acceleration value
/** \ingroup lz4_config
  */
#define XRIF_LZ4_ACCELERATION_MIN (1)

/// The maximum possible acceleration value.
/** \ingroup lz4_config
  */
#define XRIF_LZ4_ACCELERATION_MAX (65537)

/// The default LZ4 acceleration parameter
/** \ingroup lz4_config
  */
#define XRIF_LZ4_ACCELERATION_DEFAULT (1)


/// The default LZ4HC compression level
/** \ingroup lz4hc_config
  */
#define XRIF_LZ4HC_LEVEL_DEFAULT ( LZ4HC_CLEVEL_DEFAULT )

/// The default FastLZ compression level
/** \ingroup fastlz_config
  */ 
#define XRIF_FASTLZ_LEVEL_DEFAULT ( 1 )


/// The default zstd default compression level
/**
  * \ingroup zstd_confg
  */ 
#define XRIF_ZSTD_LEVEL_DEFAULT (-1)



/// Default compression level for zlib
/** Note that this has no impact for the run-length encoding strategy (Z_RLE) which is default.
  *
  * \ingroup zlib_config 
  */
#define XRIF_ZLIB_LEVEL_DEFAULT (6)

/// Default compression strategy for zlib.
/**
  * \ingroup zlib_config
  */ 
#define XRIF_ZLIB_STRATEGY_DEFAULT ( Z_RLE )

/// The window bits parameter for zlib history buffer.
/** Default value for maximum compression.
  *
  * \ingroup zlib_config
  */
#define XRIF_ZLIB_WINDOWBITS (15)

/// The memory level parameter for zlib.
/** Default value for fastest speed and maximum compression.
  *
  * \ingroup zlib_config
  */
#define XRIF_ZLIB_MEMLEVEL (9)


/// Configure for compression
/** \ingroup compress_directions
  */ 
#define XRIF_DIRECTION_COMPRESS (1)

/// Configure for decompression
/** \ingroup compress_directions
  */
#define XRIF_DIRECTION_DECOMPRESS (-1)

/// The default direction
/** \ingroup compress_directions
  */
#define XRIF_COMPRESS_DIRECTION_DEFAULT (1)




/// Return code for success.
/** \ingroup error_codes
  */
#define XRIF_NOERROR (0)

/// Return code indicating that a NULL pointer was passed.
/** \ingroup error_codes
  */
#define XRIF_ERROR_NULLPTR (-5)

/// Return code indicating that the handle was not setup.
/** \ingroup error_codes
  */
#define XRIF_ERROR_NOT_SETUP    (-10)

/// Return code indicating that an invalid size was passed.
/** \ingroup error_codes
  */
#define XRIF_ERROR_INVALID_SIZE (-20)

/// Return code indicating that an invalid type was passed.
/** \ingroup error_codes
  */
#define XRIF_ERROR_INVALID_TYPE (-22)

/// Return code indicating that an insufficient size was given.
/** \ingroup error_codes
  */
#define XRIF_ERROR_INSUFFICIENT_SIZE (-25)

/// Return code indicating a malloc failure.
/** \ingroup error_codes
  */
#define XRIF_ERROR_MALLOC (-30)

/// Return code indicating that the requested feature is not available.
/** \ingroup error_codes
  */
#define XRIF_ERROR_NOTIMPL (-100)

/// Return code indicating that a bad argument was passed.
/** \ingroup error_codes
  */
#define XRIF_ERROR_BADARG (-110)

/// Return code indicating that an invalid configuration was set
/** \ingroup error_codes
  */
#define XRIF_ERROR_INVALIDCONFIG (-115)

/// Return code indicating that a failure occurred, likely in a library call.
/** \ingroup error_codes
  */
#define XRIF_ERROR_FAILURE (-120)

/// Return code indicating that the header is bad.
/** \ingroup error_codes
  */
#define XRIF_ERROR_BADHEADER (-1000)

/// Return code indicating that a wrong version was specified.
/** \ingroup error_codes
  */
#define XRIF_ERROR_WRONGVERSION (-1010)

/// Return code indicating a library returned an error (e.g. LZ4).  The library error code may be added to this.
/** \ingroup error_codes
  */
#define XRIF_ERROR_LIBERR (-10000)

/// Standard error report.
/** \ingroup error_codes
  */
#define XRIF_ERROR_PRINT( function, msg ) fprintf(stderr, "%s: %s\n", function, msg)



/// 8-bit unsigned integer \ingroup typecodes  
#define XRIF_TYPECODE_UINT8          (1)  
/// 8-bit signed integer  \ingroup typecodes
#define XRIF_TYPECODE_INT8           (2)  
/// 16-bit unsigned integer  \ingroup typecodes
#define XRIF_TYPECODE_UINT16         (3) 
/// 16-bit signed integer  \ingroup typecodes
#define XRIF_TYPECODE_INT16          (4)  
/// 32-bit unsigned integer \ingroup typecodes
#define XRIF_TYPECODE_UINT32         (5)  
/// 32-bit signed integer \ingroup typecodes 
#define XRIF_TYPECODE_INT32          (6)  
/// 64-bit unsigned integer \ingroup typecodes
#define XRIF_TYPECODE_UINT64         (7)  
/// 64-bit signed integer \ingroup typecodes 
#define XRIF_TYPECODE_INT64          (8)  
/// IEEE 754 half-precision 16-bit (uses uint16_t for storage)  \ingroup typecodes
#define XRIF_TYPECODE_HALF           (13) 
/// IEEE 754 single-precision binary floating-point format: binary32  \ingroup typecodes
#define XRIF_TYPECODE_FLOAT          (9)  
/// IEEE 754 double-precision binary floating-point format: binary64  \ingroup typecodes
#define XRIF_TYPECODE_DOUBLE         (10) 
/// complex float  \ingroup typecodes
#define XRIF_TYPECODE_COMPLEX_FLOAT  (11) 
/// complex double \ingroup typecodes
#define XRIF_TYPECODE_COMPLEX_DOUBLE (12) 




/// The xrif library configuration structure, organizing various parameters used by the functions.
/** This structure provides for setup and management of memory allocation, though externally allocated
  * buffers can be used when desired.
  *
  * Options related to compression level and speed for the supported compressors are also provided.
  * 
  * It is intended that this structure be interacted with via the xrif_t typedef, which is a pointer
  * to xrif_handle. Values of this structure should generally be changed by one of the xrif_set_*() functions,
  * to allow for error checking and consistency.  Unless you know what you're doing, of course.
  * 
  * \todo need xrif_set_XXXX function unit tests
  * 
  * \todo LZ4 has a max input size, need to check this
  * 
  * \ingroup xrif_hacking
  */ 
typedef struct
{
   /** \name Data Dimensions
     * @{
     */
   
   xrif_dimension_t m_width;     ///< The width of a single image, in pixels
   xrif_dimension_t m_height;    ///< The height of a single image, in pixels
   xrif_dimension_t m_depth;     ///< The depth of a single image, in pixels
   xrif_dimension_t m_frames;    ///< The number of frames in the stream
   
   xrif_int_t  m_type_code;  ///< The code specifying the data type of the pixels
   
   size_t m_data_size;     ///< The size of the pixels, bytes.  This corresponds to `sizeof(type)`.

   size_t m_raw_size;         ///< Size of the stream before compression.  Set dynamically by xrif_set_size or from header.
   
   size_t m_compressed_size;  ///< Size of the stream after compression.  Set dynamically by compression functions or from header.
   
   ///@}

   /** \name xrif sequence configuration 
     * @{
     */
   
   xrif_int_t m_difference_method; ///< The difference method to use.
   
   xrif_int_t m_reorder_method;   ///< The method to use for bit reordering.
   
   xrif_int_t m_compress_method; ///< The compression method used.
   
   xrif_int_t m_compress_direction; ///< The compression direction.

   ///@}
   
   /** \name LZ4 configuration 
     * @{
     */
   
   xrif_int_t m_lz4_acceleration; ///< LZ4 acceleration parameter, >=1, higher is faster with less comporession.  Default is 1.
   
   ///@}
   
   /** \name LZ4 HC configuration 
     * @{
     */
   
   xrif_int_t m_lz4hc_level; ///< LZ4 HC compression level.  1 <= clevel <= 12.  Default is 9.

   ///@}

   /** \name FastLZ configuration 
     * @{
     */
      
   xrif_int_t m_fastlz_level; ///< FastLZ compression level. 
                       /**< According to the docs, 1 = faster, lower compression,  2 = slower, better compression.  
                         *  No other valid values.  xrif default is 2, which seems to be faster for large image cubes.
                         */

   ///@}
                     
   /** \name zlib configuration 
     * @{
     */
   
   z_stream * m_zlib_stream; ///< zlib stream structure, holds the compression/decompression configuration
                       /**< Is allocated and configured when the compression method is set to zlib.
                         */

   xrif_int_t m_zlib_level; ///< zlib compression level
                            /**< Valid values 0 to 9.  0 is no compression.  1 is fastest, 9 highest compression.
                              * The xrif default value is 6, set by XRIF_ZLIB_DEFAULT_LEVEL.
                              * Note: if m_zlib_strategy is Z_RLE (the xrif default), this parameter has no detectable effect.
                              */ 
   
   xrif_int_t m_zlib_strategy; ///< zlib compression strategy.
                               /**< Possible values
                                 * - Z_DEFAULT_STRATEGY = 0
                                 * -  Z_FILTERED  = 1
                                 * -  Z_HUFFMAN_ONLY = 2
                                 * -  Z_RLE = 3
                                 * -  Z_FIXED = 4
                                 * 
                                 * Z_RLE is the default, set by XRIF_ZLIB_DEFAULT_STRATEGY.
                                 */
   ///@}

   /** \name zstd configuration 
     * @{
     */

   ZSTD_CCtx * m_zstd_cctx; ///< zstd compression context.  

   ZSTD_DCtx * m_zstd_dctx; ///< zstd decompression context.

   xrif_int_t m_zstd_level; ///< zstd compression level

   ///@}

   /** \name OpenMP Configuration
     * @{
     */
   
   xrif_int_t m_omp_parallel;   ///< Flag controlling whether OMP parallelization is used to speed up xrif processing.  
                                /**< This has no effect if XRIF_NO_OMP is defined at compile time, 
                                  *   which completely removes OMP code. Default is 0.
                                  */
   
   xrif_int_t m_omp_numthreads; ///< Number of threads to use if omp_parallel is 1.  
                                /**< For this to be meaningful, XRIF_NO_OMP must NOT be defined at compile time, 
                                  * and XRIF_OMP_NUMTHREADS must be defined at compile time. Default is 1.
                                  */
   
   ///@}
   
   /** \name Working Memory
     * @{
     */
   
   unsigned char compress_on_raw; ///< Flag (true/false) indicating whether the raw buffer is used for compression.  Default on initialization is true.
   
   unsigned char own_raw;  ///< Flag (true/false) indicating whether the raw_buffer pointer is managed by this handle
   char * raw_buffer;      ///< The raw buffer pointer, contains the image data, and if compress_on_raw == true the compressed data.
   size_t raw_buffer_size; ///< The size of the raw_buffer pointer.  
                           /**< If `compress_on_raw` is false, then this must be at least width*height*depth*frames*data_size. If
                             * ` compress_on_raw` is true, this should be at least LZ4_compressBound(width*height*depth*frames*data_size) in size, but this is not a strict 
                             *  requirement in practice for most streams.  If this library is used to allocate it, it will be the larger of the two possibilities.
                             */
   
   unsigned char own_reordered;   ///< Flag (true/false) indicating whether the reordered_buffer pointer is managed by this handle.
   char * reordered_buffer;       ///< The reordered buffer pointer, contains the reordered data.
   size_t reordered_buffer_size;  ///< The size of the reordered_buffer pointer.  
                                  /**< It must be at least the value given by xrif_min_reordered_size(xrif_t).
                                    */
   
   unsigned char own_compressed;  ///< Flag (true/false) indicating whether the compressed_buffer pointer is managed by this handle.
   char * compressed_buffer;      ///< The compressed buffer pointer, contains the compressed data.
   size_t compressed_buffer_size; ///< The size of the compressed_buffer pointer.  
                                  /**< Depends on compression method:
                                    * - LZ4: In principle should be at least LZ4_compressBound( xrif_min_reordered_size(xrif_t) ) 
                                    *        in size, but this is not a strict requirement in practice for most streams.  It must be at least width*height*depth*frames*data_size.  
                                    *        If this library is used to allocate it, it will be the larger of the two.
                                    * - FastLZ: it should be at least 5% larger than the redordered buffer (the input to the compressor)
                                    * - none: it should be at least width*height*depth*frames*data_size.
                                    */
   ///@}                                 
                  
   /** \name Performance Measurements
     * @{ 
     */
   unsigned char calc_performance; ///< Flag (true/false) controlling whether performance metrics are calculated.  Default is true.
   
   double compression_ratio; ///< The compression ratio, calculated as output-size/input-size
   double encode_time;  ///< Time in seconds taken to encode the data
   double encode_rate; ///< Rate at which the data was encoded in bytes per second
   double difference_time; ///< Time in seconds taken to difference the data
   double difference_rate;  ///< Rate at which the data was differenced in bytes per second
   double reorder_time; ///< Time in seconds taken to reorder the data
   double reorder_rate; ///< Rate at which the data was reordered in bytes per second
   double compress_time; ///< Time in seconds taken to compress the data
   double compress_rate; ///< Rate at which the data was compressed in bytes per second
   
   struct timespec ts_difference_start; ///< Timespec used to mark the beginning of differencing, which is also the beginning of encoding
   struct timespec ts_reorder_start; ///< Timespec used to mark the beginning of reordering, which is the end of differencing
   struct timespec ts_compress_start; ///< Timespec used to mark the beginning of compression, which is the end of reordering
   struct timespec ts_compress_done; ///< Timespec used to mark the end of compression and encoding
   
   struct timespec ts_decompress_start; ///< Timespec used to mark the beginning of decompression, which is also the beginning of decoding
   struct timespec ts_unreorder_start; ///< Timespec used to mark the beginning of unreordering, which is the end of decompression
   struct timespec ts_undifference_start; ///< Timespec used to mark the beginning of undifferencing, which is the end of unreordering
   struct timespec ts_undifference_done; ///< Timespec used to mark the end of undifferencing and decoding
   
   ///@}
   
} xrif_handle;

/// The xrif handle pointer type.  This provides the main interface to the xrif library.
/** \ingroup xrif_types
  */
typedef xrif_handle* xrif_t;

/// Allocate a handle and initialize it.
/** The argument is a pointer to \ref xrif_t, making it the address of an `xrif_handle` pointer.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if a null pointer is passed
  * \returns \ref XRIF_ERROR_MALLOC on an allocation error
  * \returns \ref XRIF_NOERROR on success
  * 
  * \see xrif_reset, xrif_delete
  * 
  * \ingroup xrif_init
  */ 
xrif_error_t xrif_new(xrif_t * handle_ptr /**< [out] a pointer to an xrif handle. */);

/// Set the basic parameters of an xrif handle
/** After setting these parameters, a call to one of the allocate or set functions
  * will succeed.
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_INVALID_SIZE if any of `w`, `h`, `d`, or `f` are 0.
  * \returns \ref XRIF_ERROR_INVALID_TYPE if `c` specifies an invalid type code
  * \returns \ref XRIF_NOERROR on success
  * 
  *  \ingroup xrif_init
  */
xrif_error_t xrif_set_size( xrif_t handle,      ///< [in/out] the xrif handle to be set up
                            xrif_dimension_t w, ///< [in] the width of a single frame of data, in pixels
                            xrif_dimension_t h, ///< [in] the height of a single frame of data, in pixels
                            xrif_dimension_t d, ///< [in] the depth of a single frame of data, in pixels
                            xrif_dimension_t f, ///< [in] the number of frames of data, each frame having w X h x d pixels
                            xrif_int_t  c       ///< [in] the code specifying the data type
                          );

/// Configure the difference, reorder, and compression methods.
/** Sets the difference_method, reorder_method, and compress_method members of handle.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `difference_method`, `reorder_method`, or `compress_method` is not a valid method.  Will set that method its default.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo add direction to \ref xrif_configure
  * 
  * \ingroup xrif_init
  */ 
xrif_error_t xrif_configure( xrif_t handle,         ///< [in/out] the xrif handle to be configured
                             int difference_method, ///< [in] the new reorder method
                             int reorder_method,    ///< [in] the new reorder method
                             int compress_method    ///< [in] the new compress method
                           );

/// Allocate all memory buffers according to the configuration specified in the handle.
/** You must call xrif_set_size and xrif_configure prior to calling this function.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a null pointer
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle has not been configured
  * \returns \ref XRIF_ERROR_MALLOC on an error from `malloc`
  * \returns \ref XRIF_NOERROR on success. 
  * 
  * \ingroup xrif_init
  */ 
xrif_error_t xrif_allocate( xrif_t handle   /**< [in/out] the xrif object to be allocated */);


/// Reset a handle, restoring it to the initialized state. De-allocates owned pointers and re-initializes.
/** Free()s the raw and reordered buffers (if owned by this handle) and 
  * calls xrif_initialize_handle().  All defaults will be set.
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a null pointer
  * \returns \ref XRIF_NOERROR on success. 
  * 
  * \see xrif_new, xrif_delete
  * 
  * \ingroup xrif_init
  */
xrif_error_t xrif_reset( xrif_t handle /**< [in/out] the xrif handle */);

/// Deallocate a handle, including any memory that it owns.
/**
  * \returns \ref XRIF_ERROR_NULLPTR if a null pointer is passed
  * \returns \ref XRIF_NOERROR on success
  * 
  * \see xrif_new, xrif_reset
  * 
  * \ingroup xrif_init
  */ 
xrif_error_t xrif_delete(xrif_t handle /**< [in] an xrif handle which has been initialized with xrif_new */);

/// Initialize an xrif handle object.
/** Sets all values to defaults, and ensures that 
  * calls to allocate functions or xrif_reset_handle
  * will safely succeed.
  *
  * In general this should not be called independently, rather you should use
  * xrif_new.  If you do, this function must only be called on an xrif handle which does
  * not already have memory allocated -- otherwise memory leaks will occur! 
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if handle is NULL.
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify handle initialization. \ref tests_initialize_handle_noerror "[test doc]"
  * \test Verify xrif_initialize_handle returns error on NULL pointer. \ref tests_initialize_handle_nullptr "[test doc]"
  * 
  * \ingroup xrif_init_fine
  */ 
xrif_error_t xrif_initialize_handle( xrif_t handle /**< [out] the xrif handle to initialize */);

/// Set the difference method.
/** Sets the difference_method member of handle.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `difference_method` is not a valid difference method. Will set method to XRIF_DIFFERENCE_DEFAULT.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \ingroup xrif_init_fine
  */ 
xrif_error_t xrif_set_difference_method( xrif_t handle,        ///< [in/out] the xrif handle to be configured
                                         int difference_method ///< [in] the new reorder method
                                       );

/// Set the reorder method.
/** Sets the reorder_method member of handle.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `reorder_method` is not a valid reorder method.  Will set method to XRIF_REORDER_DEFAULT.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \ingroup xrif_init_fine
  */ 
xrif_error_t xrif_set_reorder_method( xrif_t handle,     ///< [in/out] the xrif handle to be configured
                                      int reorder_method ///< [in] the new reorder method
                                    );

/// Set the compression method and the direction.
/** Sets the compression method and sets up the relevant library interface, and for compression methods with 
  * directionality configures accordingly.
  *  
  * Valid methods are 
  * - \ref XRIF_COMPRESS_NONE
  * - \ref XRIF_COMPRESS_DEFAULT (equivalent to \ref XRIF_COMPRESS_LZ4)
  * - \ref XRIF_COMPRESS_LZ4
  * - \ref XRIF_COMPRESS_FASTLZ
  * - \ref XRIF_COMPRESS_ZLIB
  * - \ref XRIF_COMPRESS_ZSTD
  * 
  * Valid directions are:
  * - \ref XRIF_DIRECTION_COMPRESS
  * - \ref XRIF_DIRECTION_DECOMPRESS
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `compress_method` is not a valid compress method.  Will set method to XRIF_COMPRESS_DEFAULT.
  * \returns \ref XRIF_ERROR_BADARG if `compress_direction` is not a valid compress method.  Will set method to XRIF_DIRECTION_COMPRESS.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \ingroup xrif_init_fine
  */  
xrif_error_t xrif_set_compress_method_direction( xrif_t handle,                ///< [in/out] the xrif handle to be configured
                                                 xrif_int_t compress_method,   ///< [in] the new compress method
                                                 xrif_int_t compress_direction ///< [in] the new compression direction
                                               );

/// Set the compress method, and the direction to compression.
/** Sets the compression method and sets up the relevant library interface, and for compression methods with 
  * directionality sets it to compress. Calls \ref xrif_set_compress_method_direction with compress_direction=XRIF_DIRECTION_COMPRESS.
  * For details see \ref xrif_set_compress_method_direction.
  * 
  * \ingroup xrif_init_fine
  */
xrif_error_t xrif_set_compress_method( xrif_t handle,             ///< [in/out] the xrif handle to be configured
                                       xrif_int_t compress_method ///< [in] the new compress method
                                     );

/// Set the compress method, and the direction to decompression.
/** Sets the compression method and sets up the relevant library interface, and for compression methods with 
  * directionality sets it to decompress. Calls \ref xrif_set_compress_method_direction with compress_direction=XRIF_DIRECTION_DECOMPRESS.
  * For details see \ref xrif_set_compress_method_direction.
  * 
  * \ingroup xrif_init_fine
  */   
xrif_error_t xrif_set_decompress_method( xrif_t handle,             ///< [in/out] the xrif handle to be configured
                                         xrif_int_t compress_method ///< [in] the new compress method
                                       );



/// Calculate the minimum size of the raw buffer.
/** Result is based on current connfiguration of the handle.
  * 
  * If \ref compress_on_raw = 1, this returns xrif_min_compressed_size(xrif_t).
  * 
  * \returns the minimum size of the raw buffer for a valid configuration.
  * \returns 0 for an invalid configuration. 
  */
size_t xrif_min_raw_size(xrif_t handle /**< [in] the xrif handle */ );

/// Calculate the minimum size of the reordered buffer.
/** Result is based on current connfiguration of the handle.
  * 
  * \returns the minimum size of the reordered buffer for a valid configuration.
  * \returns 0 for an invalid configuration. 
  */
size_t xrif_min_reordered_size(xrif_t handle /**< [in] the xrif handle */ );

/// Calculate the minimum size of the compressed buffer for LZ4HC compression
/**
  * This uses lz4::LZ4_compressBound for the minimum reordered buffer size.
  * 
  * \returns the minimum size on success
  * \returns 0 otherwise
  */
size_t xrif_min_compressed_size_lz4hc(xrif_t handle /**< [in] the xrif handle */);

/// Calculate the minimum size of the compressed buffer for FastLZ compression
/**
  * This calculates 1.05 x the minimum reordered buffer size I.A.W. the FastLZ requirement.
  * 
  * \returns the minimum size on success
  * \returns 0 otherwise
  */
size_t xrif_min_compressed_size_fastlz(xrif_t handle /**< [in] the xrif handle */);

/// Calculate the minimum size of the compressed buffer for zlib compression
/**
  * This uses zlib::deflateBound for the minimum reordered buffer size.
  * 
  * \returns the minimum size on success
  * \returns 0 otherwise
  */
size_t xrif_min_compressed_size_zlib(xrif_t handle /**< [in] the xrif handle */);

/// Calculate the minimum size of the compressed buffer for zstd compression
/**
  * This uses zstd::ZSTD_compressBound for the minimum reordered buffer size.
  * 
  * \returns the minimum size on success
  * \returns 0 otherwise
  */
size_t xrif_min_compressed_size_zstd(xrif_t handle /**< [in] the xrif handle */);

/// Calculate the minimum size of the compressed buffer.
/** Result is based on current connfiguration of the handle.
  * 
  * The basic minimum size is set by xrif_min_reordered_size(xrif_t), since the reordered buffer is
  * what is compressed.
  * 
  * For LZ4, this calls xrif_min_compressed_size_lsz(xrif_t).
  * 
  * For FastLZ, this calls xrif_min_compressed_size_fastlz(xrif_t).
  * 
  * \returns the minimum size of the compressed buffer for a valid configuration.
  * \returns 0 for an invalid configuration or an error. 
  */
size_t xrif_min_compressed_size(xrif_t handle /**< [in] the xrif handle */ );

/// Set the raw data buffer to a pre-allocated pointer
/** Must only be called after \ref xrif_set_size and \ref xrif_configure have been called.
  * You are responsible for allocating the buffer to be at least as large as the value returned by \ref xrif_min_raw_size.
  * This will return an error if size is too small for the currently set values.
  * 
  * This pointer will not be free()-ed on a call to \ref xrif_reset_handle.
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_INVALID_SIZE if bad values are passed for raw or size
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if the size of the buffer is too small for the configured parameters
  * \returns \ref XRIF_NOERROR on success
  * 
  * \see xrif_allocate_raw
  * 
  * \ingroup xrif_init_fine
  */  
xrif_error_t xrif_set_raw( xrif_t handle,  ///< [in/out] the xrif handle
                           void * raw,     ///< [in] the pointer to a pre-allocated block
                           size_t size     ///< [in] the size of the pre-allocated block
                         );

/// Allocate the raw buffer based on the already set stream dimensions.
/** Must only be called after \ref xrif_set_size and \ref xrif_configure have been called. 
  * 
  * If xrif_handle::raw_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * The size will be set to the maximum of the pre-setup data size and (if xrif_handle::compress_on_raw == true) 
  * the LZ4_compressBound result.  LZ4 typically (in all tested cases) requests
  * a few hundred more bytes for compression.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a null pointer
  * \returns \ref XRIF_ERROR_NOT_SETUP if the width, heigh, depth, frames, and data_size parameters have not been set
  * \returns \ref XRIF_ERROR_MALLOC if malloc returns a null pointer.  In this case own_raw will be set to 0.
  * \returns \ref XRIF_NOERROR on success
  * 
  * \see xrif_set_raw
  * 
  * \ingroup xrif_init_fine
  */
xrif_error_t xrif_allocate_raw( xrif_t handle /**< [in/out] the xrif object to modify */);


/// Set the rordered (working) data buffer to a pre-allocated pointer
/** Must only be called after xrif_set_size and xrif_configure have been called. 
  * 
  * You are responsible for allocating the buffer to be at least the value returned by xrif_min_reordered_size(xrif_t).
  * 
  * This pointer will not be free()-ed on a call to xrif_reset_handle.
  *
  * \returns 0 on success
  * \returns < 0 on error, with the appropriate XRIF_ERROR_* code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_INVALID_SIZE if bad values are passed for raw or size
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if the size of the buffer is too small for the configured parameters
  * \returns \ref XRIF_NOERROR on success\todo need to have a min size calculation function exposed
  * 
  * \ingroup xrif_init_fine
  */  
xrif_error_t xrif_set_reordered( xrif_t handle,  ///< [in/out] the xrif object to modify
                                 void * reordered, ///< [in] pointer to a pre-allocated block
                                 size_t size       ///< [in] the size of the pre-allocated block
                               );

/// Allocate the reordered buffer based on the already set stream dimensions.
/** Must only be called after xrif_set_size and xrif_configure have been called. 
  * 
  * If the reordered_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a null pointer
  * \returns \ref XRIF_ERROR_NOT_SETUP if the width, heigh, depth, frames, and data_size parameters have not been set
  * \returns \ref XRIF_ERROR_MALLOC if malloc returns a null pointer.  In this case own_raw will be set to 0.
  * \returns \ref XRIF_NOERROR on success
  * 
  * \ingroup xrif_init_fine
  */
xrif_error_t xrif_allocate_reordered( xrif_t handle /**< [in/out] the xrif object to modify */);

/// Set the compressed data buffer to a pre-allocated pointer
/** Must only be called after xrif_set_size and xrif_configure have been called. 
  * 
  * You are responsible for allocating the buffer to be at least as large as xrif_min_compressed_size(xrif_t).
  * 
  * 
  * This pointer will not be free()-ed on a call to xrif_reset_handle.
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_INVALID_SIZE if bad values are passed for raw or size
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if the size of the buffer is too small for the configured parameters
  * \returns \ref XRIF_NOERROR on success\todo need to have a min size calculation function exposed
  *
  * \ingroup xrif_init_fine
  */  
xrif_error_t xrif_set_compressed( xrif_t handle,  ///< [in/out] the xrif object to modify
                                  void * reordered, ///< [in] pointer to a pre-allocated block
                                  size_t size       ///< [in] the size of the pre-allocated block
                                );

/// Allocate the compressed buffer based on the already set stream dimensions.
/** Must only be called after xrif_set_size and xrif_configure have been called. 
  * 
  * If the compressed_buffer is currently allocated and owned, it is first free()-ed.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a null pointer
  * \returns \ref XRIF_ERROR_NOT_SETUP if the width, heigh, depth, frames, and data_size parameters have not been set
  * \returns \ref XRIF_ERROR_MALLOC if malloc returns a null pointer.  In this case own_raw will be set to 0.
  * \returns \ref XRIF_NOERROR on success
  * 
  * \ingroup xrif_init_fine
  */
xrif_error_t xrif_allocate_compressed( xrif_t handle /**< [in/out] the xrif handle */);


/// Get the current width of the configured handle.
/** This simply returns the current value of handle->m_width.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the width unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */ 
xrif_dimension_t xrif_width( xrif_t handle /**< [in] the xrif handle */);

/// Get the current height of the configured handle.
/** This simply returns the current value of handle->m_height.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the height unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
xrif_dimension_t xrif_height( xrif_t handle /**< [in] the xrif handle*/);


/// Get the current depth of the configured handle.
/** This simply returns the current value of handle->m_depth.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the depth unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
xrif_dimension_t xrif_depth( xrif_t handle /**< [in] the xrif handle*/);

/// Get the current number of frames of the configured handle.
/** This simply returns the current value of handle->m_frames.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the number of frames unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
xrif_dimension_t xrif_frames( xrif_t handle /**< [in] the xrif handle*/);
   
/// Get the current type code of the configured handle.
/** This simply returns the current value of handle->m_type_code.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the type code unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
xrif_int_t xrif_type_code( xrif_t handle /**< [in] the xrif handle*/);

/// Get the current data size of the configured handle.
/** This simply returns the current value of handle->m_data_size.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the data size unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
size_t xrif_data_size( xrif_t handle /**< [in] the xrif handle*/);

/// Get the current raw size of the configured handle.
/** This simply returns the current value of handle->m_raw_size.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the raw size unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
size_t xrif_raw_size( xrif_t handle /**< [in] the xrif handle*/);

/// Get the current compressed size of the configured handle.
/** This simply returns the current value of handle->m_compressed_size.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, 0 is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the compressed size unless an error occurs, in which case 0 (which could be a valid value)
  * 
  * \ingroup access
  */
size_t xrif_compressed_size( xrif_t handle /**< [in] the xrif handle*/);

/// Get the difference method of the configured handle.
/** This simply returns the current value of handle->m_difference_method.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned. 
  * 
  * \returns the difference method unless an error occurs, in which case XRIF_ERROR_NULLPTR (which could be a valid value)
  * 
  * \ingroup access
  */
xrif_int_t xrif_difference_method( xrif_t handle /**< [in] the xrif handle*/);

/// Get the reorder method of the configured handle.
/** This simply returns the current value of handle->m_reorder_method.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the reorder method unless an error occurs, in which case XRIF_ERROR_NULLPTR.
  * 
  * \ingroup access
  */
xrif_int_t xrif_reorder_method( xrif_t handle /**< [in] the xrif handle*/);

/// Get the compress method of the configured handle.
/** This simply returns the current value of handle->m_compress_method.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the compress method unless an error occurs, in which case XRIF_ERROR_NULLPTR.
  * 
  * \ingroup access
  */
xrif_int_t  xrif_compress_method( xrif_t handle /**< [in] the xrif handle*/);

/// Get the value of the OpenMP parallel flag
/** This returns the current OpenMP parallel flag.  OpenMP multithreading is not used if < 1, used if > 0.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the value of the OpenMP parallel flag.
  * \returns XRIF_ERROR_NULLPTR if handle is NULL.
  * 
  * \ingroup access
  */
int xrif_omp_parallel( xrif_t handle /**< [in] the xrif handle*/);

/// Get the number of OpenMP threads used for parallel computations.
/** This returns the current number of OpenMP threads.  
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the number of OpenMP threads.
  * \returns XRIF_ERROR_NULLPTR if handle is NULL.
  * 
  * \ingroup access
  */
int xrif_omp_numthreads( xrif_t handle /**< [in] the xrif handle*/);

/// Populate a header buffer with the xrif protocol details.
/**
  * \returns \ref XRIF_ERROR_NULLPTR if either header or handle is NULL
  * \returns \ref XRIF_NOERROR on success 
  * 
  * \todo add zstd and zlib to header write
  * 
  * \ingroup header
  */
xrif_error_t xrif_write_header( char * header, ///< [out] the buffer to hold the protocol header. Must be at least 48 bytes long.
                                xrif_t handle  ///< [in] the xrif handle from which to populate the header.  This must have been created with xrif_new.
                              );

/// Configure an xrif handle by reading a xrif protocol header
/**
  * \returns \ref XRIF_ERROR_NULLPTR if any of the arguments are NULL
  * \returns \ref XRIF_ERROR_BADHEADER if the xrif magic number is not the first 4 bytes
  * \returns \ref XRIF_ERROR_WRONGVERSION if the XRIF version in the header is too high for the compiled library
  * \returns \ref XRIF_NOERROR on success
  * 
  * \todo add zstd and zlib to header read
  * 
  * \ingroup header
  */
xrif_error_t xrif_read_header( xrif_t handle,          ///< [out] the xrif header to configure.  This must have been created with xrif_new.
                               uint32_t * header_size, ///< [out] the total size of the header, read from the buffer.
                               char * header           ///< [in] the buffer containing the header
                             );



/// Encode data using the xrif format 
/** Calls \ref xrif_difference(), \ref xrif_reorder(), and \ref xrif_compress().
  * If any of these returns an error, that error is returned.
  * If all methods are NONE, this is a no-op and returns immediately.
  * 
  * The timespecs are updated during this call.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOTIMPL if any of xrif_handle::difference_method, xrif_handle::reorder_method, or xrif_handle::compress_method as set in the handle are not valid
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either xrif_handle::raw_buffer or xrif_handle::reordered_buffer are not of sufficient size for the configured handle
  * \returns \ref XRIF_NOERROR on success
  * 
  * \see xrif_decode
  * 
  * \ingroup xrif_encode_decode
  */
xrif_error_t xrif_encode( xrif_t handle /**< [in/out] the xrif handle */);

/// Decode data from the xrif format 
/** Calls \ref xrif_decompress, \ref xrif_unreorder, and \ref xrif_undifference.
  * If any of these returns an error, that error is returned.
  * 
  * The timespecs are updated during this call.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOTIMPL if any of xrif_handle::difference_method, xrif_handle::reorder_method, or xrif_handle::compress_method as set in the handle are not valid
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either xrif_handle::raw_buffer or xrif_handle::reordered_buffer are not of sufficient size for the configured handle
  * \returns \ref XRIF_NOERROR on success
  *
  * \see xrif_encode 
  * 
  * \ingroup xrif_encode_decode
  */
xrif_error_t xrif_decode( xrif_t handle /**< [in/out] the xrif handle */);

/// Difference the image(s)
/** This function calls the method specific difference function for the method specified by
  * handle->difference_method.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOTIMPL if xrif_handle::difference_method as set in the handle is not valid
  * \returns other error codes from the differencing functions
  * \returns \ref XRIF_NOERROR on success
  * 
  * \ingroup xrif_diff
  */
xrif_error_t xrif_difference( xrif_t handle /**< [in/out] the xrif handle */ );

/// Undifference the image(s)
/** This function calls the method specific undifference function for the method specified by
  * handle->difference_method.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOTIMPL if xrif_handle::difference_method as set in the handle is not valid
  * \returns other error codes from the differencing functions
  * \returns \ref XRIF_NOERROR on success
  * 
  * \ingroup xrif_diff
  */
xrif_error_t xrif_undifference( xrif_t handle /**< [in/out] the xrif handle */ );

/// Difference the images using the previous image as a reference.
/** This function calls the type specific difference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if differencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify previous differencing for int16_t \ref diff_previous_int16_white "[test doc]"
  * \test Verify previous differencing for uint16_t \ref diff_previous_uint16_white "[test doc]"
  * \test Verify previous differencing for int32_t \ref diff_previous_int32_white "[test doc]"
  * \test Verify previous differencing for uint32_t \ref diff_previous_uint32_white "[test doc]"
  * \test Verify previous differencing for int64_t \ref diff_previous_int64_white "[test doc]"
  * \test Verify previous differencing for uint64_t \ref diff_previous_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_previous
  */
xrif_error_t xrif_difference_previous( xrif_t handle /**< [in/out] the xrif handle */ );

/// Difference the images using the first image as a reference.
/** This function calls the type specific difference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if differencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify first differencing for int16_t \ref diff_first_int16_white "[test doc]"
  * \test Verify first differencing for uint16_t \ref diff_first_uint16_white "[test doc]"
  * \test Verify first differencing for int32_t \ref diff_first_int32_white "[test doc]"
  * \test Verify first differencing for uint32_t \ref diff_first_uint32_white "[test doc]"
  * \test Verify first differencing for int64_t \ref diff_first_int64_white "[test doc]"
  * \test Verify first differencing for uint64_t \ref diff_first_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_first
  */
xrif_error_t xrif_difference_first( xrif_t handle /**< [in/out] the xrif handle */ );

/// Difference the images using the previous pixel as a reference.
/** This function calls the type specific difference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if differencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify pixel differencing for int16_t \ref diff_pixel_int16_white "[test doc]"
  * \test Verify pixel differencing for uint16_t \ref diff_pixel_uint16_white "[test doc]"
  * \test Verify pixel differencing for int32_t \ref diff_pixel_int32_white "[test doc]"
  * \test Verify pixel differencing for uint32_t \ref diff_pixel_uint32_white "[test doc]"
  * \test Verify pixel differencing for int64_t \ref diff_pixel_int64_white "[test doc]"
  * \test Verify pixel differencing for uint64_t \ref diff_pixel_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_pixel
  */
xrif_error_t xrif_difference_pixel( xrif_t handle /**< [in/out] the xrif handle */ );

/// Difference the images using the previous pixel as a reference.
/** This function calls the type specific difference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if differencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \ingroup xrif_diff_bayer
  */ 
xrif_error_t xrif_difference_bayer( xrif_t handle /**< [in/out] the xrif handle */ );

/// Undifference the images using the previous image as a reference.
/** This function calls the type specific undifference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if undifferencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify previous differencing for int16_t \ref diff_previous_int16_white "[test doc]"
  * \test Verify previous differencing for uint16_t \ref diff_previous_uint16_white "[test doc]"
  * \test Verify previous differencing for int32_t \ref diff_previous_int32_white "[test doc]"
  * \test Verify previous differencing for uint32_t \ref diff_previous_uint32_white "[test doc]"
  * \test Verify previous differencing for int64_t \ref diff_previous_int64_white "[test doc]"
  * \test Verify previous differencing for uint64_t \ref diff_previous_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_previous
  */
xrif_error_t xrif_undifference_previous( xrif_t handle /**< [in/out] the xrif handle */ );

/// Undifference the images using the first image as a reference.
/** This function calls the type specific undifference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if undifferencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify first differencing for int16_t \ref diff_first_int16_white "[test doc]"
  * \test Verify first differencing for uint16_t \ref diff_first_uint16_white "[test doc]"
  * \test Verify first differencing for int32_t \ref diff_first_int32_white "[test doc]"
  * \test Verify first differencing for uint32_t \ref diff_first_uint32_white "[test doc]"
  * \test Verify first differencing for int64_t \ref diff_first_int64_white "[test doc]"
  * \test Verify first differencing for uint64_t \ref diff_first_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_first
  */
xrif_error_t xrif_undifference_first( xrif_t handle /**< [in/out] the xrif handle */ );

/// Undifference the images using the previous pixel as a reference.
/** This function calls the type specific undifference function for the type specified by
  * handle->type_code.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if the handle is NULL
  * \returns \ref XRIF_ERROR_NOT_SETUP if the handle is not configured
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if raw_buffer_size is not big enough given the configuration
  * \returns \ref XRIF_ERROR_NOTIMPL if undifferencing is not implemented for the type specified in xrif_handle::type_code
  * \returns \ref XRIF_NOERROR on success
  * 
  * \test Verify pixel differencing for int16_t \ref diff_pixel_int16_white "[test doc]"
  * \test Verify pixel differencing for uint16_t \ref diff_pixel_uint16_white "[test doc]"
  * \test Verify pixel differencing for int32_t \ref diff_pixel_int32_white "[test doc]"
  * \test Verify pixel differencing for uint32_t \ref diff_pixel_uint32_white "[test doc]"
  * \test Verify pixel differencing for int64_t \ref diff_pixel_int64_white "[test doc]"
  * \test Verify pixel differencing for uint64_t \ref diff_pixel_uint64_white "[test doc]"
  * 
  * \ingroup xrif_diff_pixel
  */
xrif_error_t xrif_undifference_pixel( xrif_t handle /**< [in/out] the xrif handle */ );



/// Reorder the data using the method specified by `reorder_method`
/**
  * \returns \ref XRIF_ERROR_NONE on success
  * \returns an error code on an error.
  * 
  * \ingroup xrif_reorder
  */ 
xrif_error_t xrif_reorder( xrif_t handle /**< [in/out] the xrif handle */);

/// Un-reorder the data using the method specified by `reorder_method`
/**
  * \returns \ref XRIF_ERROR_NONE on success
  * \returns an error code on an error.
  * 
  * \ingroup xrif_reorder
  */ 
xrif_error_t xrif_unreorder( xrif_t handle /**< [in/out] the xrif handle */);



/// Perform no re-ordering, simply copy raw to reordered.
/** Also zeroes any excess in xrif_handle::reordered_buffer.
  *
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if allocated buffers aren't big enough
  * 
  * \ingroup xrif_reorder_none
  */ 
xrif_error_t xrif_reorder_none( xrif_t handle /**< [in/out] the xrif handle */ );




/// Dispatch bytepack reodering based on type
/** Calls the type appropriate xrif_reorder_bytepack_X function.
  *
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_reorder_bytepack( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform bytepack reodering for signed 16 bit ints
/** 
  *
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_reorder_bytepack_sint16( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform bytepack-renibble reordering
/** \ingroup xrif_reorder_bytepack_renibble
  *
  * \todo document
  */
xrif_error_t xrif_reorder_bytepack_renibble( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform bytepack-renibble reordering for signed int16
/** \ingroup xrif_reorder_bytepack_renibble
  * \todo document
  */
xrif_error_t xrif_reorder_bytepack_renibble_sint16( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform bitpack reordering
/** \ingroup xrif_reorder_bitpack
  * \todo document
  */
xrif_error_t xrif_reorder_bitpack( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform bitpack reordering for signed int16
/** \ingroup xrif_reorder_bitpack
  * \todo document
  */      
xrif_error_t xrif_reorder_bitpack_sint16( xrif_t handle /**< [in/out] the xrif handle */ );

/// Perform no un-re-ordering, simply copy reordered to raw.
/** Also zeroes any excess in xrif_handle::raw_buffer.
  *
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if allocated buffers aren't big enough
  * 
  * \ingroup xrif_reorder_none
  */ 
xrif_error_t xrif_unreorder_none( xrif_t handle /**< [in/out] the xrif handle */);

/// Dispatch bytepack unreodering based on type
/** Calls the type appropriate xrif_unreorder_bytepack_X function.
  *
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_unreorder_bytepack( xrif_t handle /**< [in/out] the xrif handle */);

/// Perform bytepack unreodering for signed 16 bit ints
/** 
  * \todo this does not actually perform any size checks, but should.
  * 
  * \returns \ref XRIF_NOERROR on success
  * \returns \ref XRIF_ERROR_NULLPTR if handle is null.
  * \returns \ref XRIF_ERROR_INSUFFICIENT_SIZE if either raw_buffer or reorderd_buffer aren't big enough
  * 
  * \ingroup xrif_reorder_bytepack
  */ 
xrif_error_t xrif_unreorder_bytepack_sint16( xrif_t handle /**< [in/out] the xrif handle */);

/// Perform bytepack-renibble unreordering
/** \todo document
  *
  * \ingroup xrif_reorder_bytepack_renibble
  */ 
xrif_error_t xrif_unreorder_bytepack_renibble( xrif_t handle /**< [in/out] the xrif handle */);

/// Perform bitpack unreordering
/** \todo document
  *
  * \ingroup xrif_reorder_bitpack
  */
xrif_error_t xrif_unreorder_bitpack( xrif_t handle /**< [in/out] the xrif handle */);

//==========================================================
//             Compression
//==========================================================

/// Dispatch compression based on method
/** Calls the relevant compression function based on the value of handle->compress_method.
  *
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_NOTIMPL if an unknown compression method is configured in handle
  * \returns error codes from the specific compression methods.
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup xrif_compress
  */ 
xrif_error_t xrif_compress( xrif_t handle /**< [in/out] the xrif handle */);

/// Dispatch decompression based on method
/** Calls the relevant decompression function based on the value of handle->compress_method.
  *
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_NOTIMPL if an unknown compression method is configured in handle
  * \returns error codes from the specific decompression methods.
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup xrif_compress
  */
xrif_error_t xrif_decompress(xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     No Compression
//----------------------------------------------------------

/// Compress using the no compression method
/** This is simply a memory copy.
  *
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_INSUFFICIENT_SIZE if the destination buffer is not large enough
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_none
  */
xrif_error_t xrif_compress_none( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using the no compression method
/** This is simply a memory copy.
  *
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_INSUFFICIENT_SIZE if the destination buffer is not large enough
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_none
  */
xrif_error_t xrif_decompress_none( xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     LZ4 Compression
//----------------------------------------------------------

/// Set the LZ4 acceleration parameter
/** The LZ4 acceleration parameter is a number greater than or equal to 1.  Larger values speed up the compression
  * process, but with less size reduction.  The default and minimum value is 1 (XRIF_LZ4_ACCEL_MIN).  
  * The maximum value is 65537 (XRIF_LZ4_ACCEL_MAX).  The LZ4 docs claim a +-3% improvement in speed for each increment.
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `lz4_accel` is out of range.  Will set value to correspondling min or max limit.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo this should not return badarg on out of range, just clamp
  * 
  * \ingroup compress_lz4
  */ 
xrif_error_t xrif_set_lz4_acceleration( xrif_t handle,    ///< [in/out] the xrif handle to be configured
                                        int32_t lz4_accel ///< [in] LZ4 acceleration parameter
                                      );

/// Get the LZ4 acceleration of the configured handle.
/** This returns the current value of the LZ4 acceleration.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the LZ4 acceleration unless an error occurs, in which case XRIF_ERROR_NULLPTR.
  * 
  * \ingroup compress_lz4
  */
xrif_int_t xrif_lz4_acceleration( xrif_t handle /**< [in] the xrif handle*/);

/// Calculate the minimum size of the compressed buffer for LZ4 compression
/**
  * This uses lz4::LZ4_compressBound for the minimum reordered buffer size.
  * 
  * \returns the minimum size on success
  * \returns 0 otherwise
  * 
  * \ingroup compress_lz4
  */
size_t xrif_min_compressed_size_lz4(xrif_t handle /**< [in] the xrif handle */);

/// Compress using LZ4
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_INSUFFICIENT_SIZE if the destination buffer is not large enough
  * \returns XRIF_ERROR_FAILURE if lz4_compress_fast returns 0
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_lz4
  */
xrif_error_t xrif_compress_lz4( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using LZ4
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_LIBERR+LZ4CODE if LZ4_decompress_safe fails
  * \returns XRIF_ERROR_INVALID_SIZE if the decompressed data does not match the expected size
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_lz4
  */
xrif_error_t xrif_decompress_lz4( xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     LZ4HC Compression
//----------------------------------------------------------

/// Set the LZ4HC compression level
/** The LZ4HC compression level is a number greater than or equal to 1 and less than or equal to 12.  Larger values should result in
  * better  compression, but with slower speeds.  The default value is 9 (XRIF_LZ4HC_CLEVEL_DEFAULT).  
  * The maximum value is 12 (XRIF_LZ4HC_CLEVEL_MAX ).  
  *
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `lz4hc_clevel` is out of range.  Will set value to correspondling min or max limit.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo this should not return badarg on out of range, just clamp
  * 
  * \ingroup compress_lz4hc
  */ 
xrif_error_t xrif_set_lz4hc_level( xrif_t handle,       ///< [in/out] the xrif handle to be configured
                                   int32_t lz4hc_clevel ///< [in] LZ4HC compression level
                                 );

/// Get the LZ4 HC compression level of the configured handle.
/** This returns the current value of the LZ4 HC compression level.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the LZ4 HC compression level unless an error occurs, in which case XRIF_ERROR_NULLPTR.
  * 
  * \ingroup compress_lz4hc
  */
xrif_int_t xrif_lz4hc_level( xrif_t handle /**< [in] the xrif handle*/);


/// Compress using LZ4HC
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_INSUFFICIENT_SIZE if the destination buffer is not large enough
  * \returns XRIF_ERROR_FAILURE if lz4_compress_fast returns 0
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_lz4hc
  */
xrif_error_t xrif_compress_lz4hc( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using LZ4HC
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_LIBERR+LZ4CODE if LZ4_decompress_safe fails
  * \returns XRIF_ERROR_INVALID_SIZE if the decompressed data does not match the expected size
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_lz4hc
  */
xrif_error_t xrif_decompress_lz4hc( xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     FastLZ Compression
//----------------------------------------------------------

/// Set the FastLZ compression level
/** The FastLZ compression level can be either 1 or 2. 1 is faster but lower compression, 2 is slower but better compression.  
  * The FastLZ docs say that 1 will be generally useful on short data.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `fastlz_lev` is out of range.  Will set value to 1.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo this should not return badarg on out of range, just clamp
  * 
  * \ingroup compress_fastlz
  */ 
xrif_error_t xrif_set_fastlz_level( xrif_t handle,     ///< [in/out] the xrif handle to be configured
                                    int32_t fastlz_lev ///< [in] FastLZ level
                                  );

/// Get the FastLZ compression level of the configured handle.
/** This returns the current value of the FastLZ compression level.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the FastLZ compression level unless an error occurs, in which case XRIF_ERROR_NULLPTR.
  * 
  * \ingroup compress_fastlz
  */
xrif_int_t xrif_fastlz_level( xrif_t handle /**< [in] the xrif handle*/);

/// Compress using FastLZ
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_INSUFFICIENT_SIZE if the destination buffer is not large enough
  * \returns XRIF_ERROR_FAILURE if fastlz_compress_level returns 0
  * \returns XRIF_NOERROR on 
  * 
  * \ingroup compress_fastlz
  */
xrif_error_t xrif_compress_fastlz( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using FastLZ
/**
  * \returns XRIF_ERROR_NULLPTR if handle is null
  * \returns XRIF_ERROR_FAILURE if fastlz_decompress fails
  * \returns XRIF_ERROR_INVALID_SIZE if the decompressed data does not match the expected size
  * \returns XRIF_NOERROR on success
  * 
  * \ingroup compress_fastlz
  */
xrif_error_t xrif_decompress_fastlz( xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     zstd Compression
//----------------------------------------------------------

/// Allocate and configure the `zstd` context
/** The `zstd` context structure for the configured direction is allocated and configured.
  * If direction is compression, the compression level is set in the context structure.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if \p handle is a NULL pointer.
  * \returns \ref XRIF_ERROR_INAVALIDCONFIG if an invalid direction is specified in handle.
  * \returns \ref XRIF_ERROR_LIBERR + code on an error from `zstd`, where code is the `zstd` error code.
  * \returns \ref XRIF_ERROR_MALLOC if an allocation error occurs.  Check errno in this case.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo move to private
  * 
  * \ingroup compress_zstd
  */
xrif_error_t xrif_setup_zstd(xrif_t handle );

/// De-allocate the `zstd` context
/** The `zstd` context structures owned by handle are free()-ed using the `zstd` library calls.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if \p handle is a NULL pointer.
  * \returns \ref XRIF_ERROR_INAVALIDCONFIG if an invalid direction is specified in handle.
  * \returns \ref XRIF_ERROR_LIBERR + code on an error from `zstd`, where code is the `zstd` error code.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo move to private
  * 
  * \ingroup compress_zstd
  */
xrif_error_t xrif_shutdown_zstd(xrif_t handle );

/// Set the zstd compression level
/** The zstd compression level can be any value from ZSTD_minCLevel() (currently -131072) to ZSTD_maxCLevel() (currently 22). 
  * The lower the number the faster but with lower compression.  Setting 0 is equivalent to ZSTD_CLEVEL_DEFAULT (currently 3).
  * Note that the xrif default, XRIF_ZSTD_LEVEL_DEFAULT, is 1. 
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if `handle` is a NULL pointer
  * \returns \ref XRIF_ERROR_BADARG if `zstd_lev` is out of range.  Will set value to 1.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \ingroup compress_zstd
  */ 
xrif_error_t xrif_set_zstd_level( xrif_t handle,   ///< [in/out] the xrif handle to be configured
                                  int32_t zstd_lev ///< [in] new zstd level
                                );

/// Get the zstd compression context structure of the configured handle.
/** This returns the current pointer to the zstd compression context structure.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, NULL is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the zstd context structure pointer;
  * \returns NULL if handle is NULL or uninitialized
  * 
  * \ingroup compress_zstd
  */
ZSTD_CCtx * xrif_zstd_cctx( xrif_t handle /**< [in] the xrif handle*/);

/// Get the zstd decompression context structure of the configured handle.
/** This returns the current pointer to the zstd decompression context structure.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, NULL is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the zstd context structure pointer;
  * \returns NULL if handle is NULL or uninitialized
  * 
  * \ingroup compress_zstd
  */
ZSTD_DCtx * xrif_zstd_dctx( xrif_t handle /**< [in] the xrif handle*/);

/// Get the zstd compression level of the configured handle.
/** This returns the current zstd compression level.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned. 
  * 
  * \returns the zstd compression level unless an error occurs.
  * \returns XRIF_ERROR_NULLPTR if handle is NULL. 
  * 
  * \ingroup compress_zstd
  */
xrif_int_t xrif_zstd_level( xrif_t handle /**< [in] the xrif handle*/);

/// Compress using zstd deflate
/** \todo document
  *
  * \ingroup compress_zstd
  */
xrif_error_t xrif_compress_zstd( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using zstd deflate
/** \todo document
  *
  * \ingroup compress_zstd
  */
xrif_error_t xrif_decompress_zstd( xrif_t handle /**< [in/out] the xrif handle */);

//----------------------------------------------------------
//     zlib Compression
//----------------------------------------------------------

/// Allocate and configure the `zlib` stream structure
/** The `zlib` stream structure is configured for either compression (deflate) or decompression (inflate)
  * according to the current compression direction.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if \p handle is a NULL pointer.
  * \returns \ref XRIF_ERROR_INAVALIDCONFIG if an invalid direction is specified in handle.
  * \returns \ref XRIF_ERROR_LIBERR + code on an error from `zlib`, where code is the `zlib` error code.
  * \returns \ref XRIF_ERROR_MALLOC if an allocation error occurs.  Check errno in this case.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo move to private
  * 
  * \ingroup compress_zlib
  */
xrif_error_t xrif_setup_zlib(xrif_t handle );

/// De-allocate the `zlib` stream structure
/** The `zlib` end function is called and the `z_stream` structure owned by handle is free()-ed.
  * 
  * \returns \ref XRIF_ERROR_NULLPTR if \p handle is a NULL pointer.
  * \returns \ref XRIF_ERROR_INAVALIDCONFIG if an invalid direction is specified in handle.
  * \returns \ref XRIF_ERROR_LIBERR + code on an error from `zlib`, where code is the `zlib` error code.
  * \returns \ref XRIF_NOERROR on success.
  * 
  * \todo move to private
  * 
  * \ingroup compress_zlib
  */
xrif_error_t xrif_shutdown_zlib(xrif_t handle );

/// Get the zlib stream structure of the configured handle.
/** This returns the current pointer to the zlib stream structure.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, NULL is returned.  This result could also indicate
  * an uninitialized handle.  If further discrimination is needed you must check handle before calling this function.
  * 
  * \returns the zlib stream structure pointer
  * \returns NULL if handle is NULL or uninitialized
  * 
  * \ingroup compress_zlib
  */
z_stream * xrif_zlib_stream( xrif_t handle /**< [in] the xrif handle*/);

/// Get the zlib compression level of the configured handle.
/** This returns the current zlib compression level.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned. 
  * 
  * \returns the zlib compression level unless an error occurs.
  * \returns XRIF_ERROR_NULLPTR if handle is NULL. 
  * 
  * \ingroup compress_zlib
  */
xrif_int_t xrif_zlib_level( xrif_t handle /**< [in] the xrif handle*/);

/// Get the zlib compression strategy of the configured handle.
/** This returns the current zlib compression strategy.
  *
  * Error handling: the only possible error is that \p handle is NULL.  In this case, XRIF_ERROR_NULLPTR is returned.
  * 
  * \returns the zlib compression level unless an error occurs.
  * \returns XRIF_ERROR_NULLPTR if handle is NULL.
  * 
  * \ingroup compress_zlib
  */
xrif_int_t xrif_zlib_strategy( xrif_t handle /**< [in] the xrif handle*/);

/// Compress using zlib deflate
/** \todo document
  *
  * \ingroup compress_zlib
  */ 
xrif_error_t xrif_compress_zlib( xrif_t handle /**< [in/out] the xrif handle */);

/// Decompress using zlib deflate
/** \todo document
  *
  * \ingroup compress_zlib
  */
xrif_error_t xrif_decompress_zlib( xrif_t handle /**< [in/out] the xrif handle */);

//==========================================================
//             Performance
//==========================================================

/// Calculate the compression ratio 
/** 
  * \returns the ratio of compressed_size to raw_size.
  * 
  * \ingroup xrif_performance
  */ 
double xrif_compression_ratio( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to encode the data
/**
  * \returns the difference in the timespecs marking the beginning of differencing the end of compression 
  * 
  * \ingroup xrif_performance
  */ 
double xrif_encode_time ( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the encode rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_encode_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_encode_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to difference the data
/**
  * \returns the difference in the timespecs marking the beginning of reordering and the beginning of differencing 
  * 
  * \ingroup xrif_performance
  */ 
double xrif_difference_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the differencing rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_difference_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_difference_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to reorder the data
/**
  * \returns the difference in the timespecs marking the beginning of compression and the beginning of reordering
  * 
  * \ingroup xrif_performance
  */ 
double xrif_reorder_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the reordering rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_reorder_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_reorder_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to compress the differenced and reordered data
/**
  * \returns the difference in the timespecs marking the beginning of reordering and the end of compression
  * 
  * \ingroup xrif_performance
  */ 
double xrif_compress_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the compression rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_compress_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_compress_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to decode the data
/**
  * \returns the difference in the timespecs marking the beginning of decompression and the end of undifferencing 
  * 
  * \ingroup xrif_performance
  */ 
double xrif_decode_time ( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the decode rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_decode_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_decode_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to undifference the data
/**
  * \returns the difference in the timespecs marking the end of undifferencing and the end of unreodering
  * 
  * \ingroup xrif_performance
  */ 
double xrif_undifference_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the undifferencing rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_undifference_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_undifference_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to unreorder the data
/**
  * \returns the difference in the timespecs marking the beginning of differencing and the end of decompression
  * 
  * \ingroup xrif_performance
  */ 
double xrif_unreorder_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the unreordering rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_unreorder_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_unreorder_rate( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the time in seconds taken to decompress the differenced and reordered data
/**
  * \returns the difference in the timespecs marking the decompression time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_decompress_time( xrif_t handle /**< [in/out] the xrif handle */);

/// Calculate the decompression rate in bytes/sec
/**
  * \returns the ratio of raw_size to xrif_decompress_time
  * 
  * \ingroup xrif_performance
  */ 
double xrif_decompress_rate( xrif_t handle /**< [in/out] the xrif handle */);

//==========================================================
//             Utilities
//==========================================================

/// Return the size of the type specified by the code.
/**
  * \returns the equivalent to `sizeof(type)` for the specified type code.
  * \returns 0 if the `type_code` is invalid
  * 
  * \ingroup xrif_utils
  */  
size_t xrif_typesize( xrif_int_t type_code /**< [in] the type code*/);

/// Calculate the difference between two timespecs.
/** Calculates `ts1-ts0` in `double` precision.
  * 
  * \returns the difference in seconds between the two timespecs
  * 
  * \ingroup xrif_utils 
  */
double xrif_ts_difference( struct timespec * ts1, ///< [in] the end time
                           struct timespec * ts0  ///< [in] the start time
                         );

/// Get a string describing the difference method
/** Returns a pointer to a string describing the difference method.
  *
  * \returns a pointer to the string if `diff_method` is valid
  * \returns NULL if `diff_method` is not valid
  * 
  * \ingroup xrif_utils
  */  
const char * xrif_difference_method_string( int diff_method /**< [in] the difference method */);

/// Get a string describing the reordering method
/** Returns a pointer to a string describing the reordering method.
  *
  * \returns a pointer to the string if `reorder_method` is valid
  * \returns NULL if `reorder_method` is not valid
  * 
  * \ingroup xrif_utils
  */  
const char * xrif_reorder_method_string( int reorder_method /**< [in] the reorder method*/);

/// Get a string describing the compression method
/** Returns a pointer to a string describing the compression method.
  *
  * \returns a pointer to the string if `compress_method` is valid
  * \returns NULL if `compress_method` is not valid
  * 
  * \ingroup xrif_utils
  */  
const char * xrif_compress_method_string( int compress_method /**< [in] the compression method*/);

#ifdef __cplusplus
//extern "C"
}
#endif
   
#endif //xrif_xrif_h
