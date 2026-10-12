/** \file xrif.c
  * \brief The eXtreme-ao Reordered Image Format: Definitions
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2019, 2020, 2021 The Arizona Board of Regents on behalf of The
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

#include "xrif.h"
#include "xrif_size_internal.h"

#define BIT15 (32768)
#define BIT14 (16384)
#define BIT13 (8192)
#define BIT12 (4096)
#define BIT11 (2048)
#define BIT10 (1024)
#define BIT09 (512)
#define BIT08 (256)
#define BIT07 (128)
#define BIT06 (64)
#define BIT05 (32)
#define BIT04 (16)
#define BIT03 (8)
#define BIT02 (4)
#define BIT01 (2)
#define BIT00 (1)

// Allocate an xrif_handle object, which is pointed to by an xrif_t
xrif_error_t xrif_new(xrif_t * handle_ptr)
{
   if( handle_ptr == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_new", "can not allocate null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   *handle_ptr = (xrif_t) malloc( sizeof(xrif_handle) );
   
   if( *handle_ptr == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_new", "error in malloc");
      XRIF_ERROR_MALLOC;
   }
   
   return xrif_initialize_handle(*handle_ptr);
   
}

// Set the basic parameters of an xrif handle
xrif_error_t xrif_set_size(
    xrif_t handle, xrif_dimension_t w, xrif_dimension_t h, xrif_dimension_t d, xrif_dimension_t f, xrif_int_t c )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_geometry_make( &geometry, w, h, d, f, c );
    if( rv != XRIF_NOERROR )
        return rv;
    handle->m_width     = w;
    handle->m_height    = h;
    handle->m_depth     = d;
    handle->m_frames    = f;
    handle->m_type_code = c;
    handle->m_data_size = geometry.m_element_bytes;
    handle->m_raw_size  = geometry.m_raw_bytes;
    return XRIF_NOERROR;
}

// Configure the difference, reorder, and compression methods.
xrif_error_t xrif_configure( xrif_t handle,      
                             int difference_method,
                             int reorder_method,
                             int compress_method
                           )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_configure", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   xrif_error_t rv1 = xrif_set_difference_method(handle, difference_method);
   
   xrif_error_t rv2 = xrif_set_reorder_method(handle, reorder_method);
   
   xrif_error_t rv3 = xrif_set_compress_method(handle, compress_method);
   
   if(rv1 != XRIF_NOERROR) return rv1;
   if(rv2 != XRIF_NOERROR) return rv2;
   if(rv3 != XRIF_NOERROR) return rv3;
   
   return XRIF_NOERROR;
   
}

// Allocate all memory buffers according to the configuration specified in the handle.
xrif_error_t xrif_allocate( xrif_t handle )
{
   if( handle == NULL) 
   {
      XRIF_ERROR_PRINT("xrif_allocate", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   xrif_geometry geometry;
   xrif_error_t  rv = xrif_geometry_get( handle, &geometry );
   if( rv != XRIF_NOERROR )
       return rv;
   if( xrif_min_raw_size( handle ) == 0 || xrif_min_reordered_size( handle ) == 0 ||
       xrif_min_compressed_size( handle ) == 0 )
       return XRIF_ERROR_INVALID_SIZE;

   rv = xrif_allocate_raw(handle);
   if(rv < 0) 
   {
      XRIF_ERROR_PRINT("xrif_allocate", "error from xrif_allocate_raw");
      return rv;
   }
   rv = xrif_allocate_reordered(handle);
   if(rv < 0) 
   {
      XRIF_ERROR_PRINT("xrif_allocate", "error from xrif_allocate_reordered");
      return rv;
   }
   
   if(handle->m_compress_on_raw == 0)
   {
      rv = xrif_allocate_compressed(handle);
      if(rv < 0) 
      {
         XRIF_ERROR_PRINT("xrif_allocate", "error from xrif_allocate_compressed");
         return rv;
      }
   }
   
   return XRIF_NOERROR;

}

// Reset a handle, restoring it to the initialized state. De-allocates owned pointers and re-initializes.
xrif_error_t xrif_reset( xrif_t handle )
{
   int rv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_allocate", "can not reset null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_reordered_buffer && handle->m_own_reordered)
   {
      free(handle->m_reordered_buffer);
   }
   
   if(handle->m_raw_buffer && handle->m_own_raw )
   {
      free(handle->m_raw_buffer);
   }
   
   if(handle->m_compressed_buffer && handle->m_own_compressed)
   {
      free(handle->m_compressed_buffer);
   }
   
   xrif_shutdown_zlib(handle);
   
   xrif_shutdown_zstd(handle);

   rv = xrif_initialize_handle(handle);
   
   if(rv != XRIF_NOERROR)
   {
      XRIF_ERROR_PRINT("xrif_reset", "error from xrif_initialize_handle");
   }
   
   return rv;
}

// De-allocate an xrif_handle object, which is pointed to by an xrif_t
xrif_error_t xrif_delete(xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_delete", "can not delete null pointer.");
      return XRIF_ERROR_NULLPTR;
   }
   
   xrif_error_t rv = xrif_reset( handle );
   
   free(handle);
   
   return rv;
}

// Initialize an xrif handle object.
xrif_error_t xrif_initialize_handle( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_initialize_handle", "can not initialize null pointer"); 
      return XRIF_ERROR_NULLPTR;
   }
   
   handle->m_width = 0;
   handle->m_height = 0;
   handle->m_depth = 0;
   handle->m_frames = 0;
   
   handle->m_type_code = 0;
   
   handle->m_data_size = 0;

   handle->m_raw_size = 0;
   handle->m_compressed_size = 0;
   
   handle->m_difference_method = XRIF_DIFFERENCE_DEFAULT;
   
   handle->m_reorder_method = XRIF_REORDER_DEFAULT;
   
   handle->m_compress_method = XRIF_COMPRESS_DEFAULT;
   
   handle->m_compress_direction = XRIF_COMPRESS_DIRECTION_DEFAULT;

   //LZ4
   handle->m_lz4_acceleration = XRIF_LZ4_ACCELERATION_DEFAULT;

   //LZ4HC
   handle->m_lz4hc_level = XRIF_LZ4HC_LEVEL_DEFAULT;

   //FastLZ
   handle->m_fastlz_level = XRIF_FASTLZ_LEVEL_DEFAULT;

   //zlib
   handle->m_zlib_stream = NULL;
   handle->m_zlib_level = XRIF_ZLIB_LEVEL_DEFAULT;
   handle->m_zlib_strategy = XRIF_ZLIB_STRATEGY_DEFAULT;

   //zstd
   handle->m_zstd_cctx = NULL;
   handle->m_zstd_dctx = NULL;
   handle->m_zstd_level = XRIF_ZSTD_LEVEL_DEFAULT;

   //OpenMP
   handle->m_omp_parallel = 0;
   handle->m_omp_numthreads = 1;
   
   handle->m_compress_on_raw = 1;
   
   handle->m_own_raw = 0;
   handle->m_raw_buffer = 0;
   handle->m_raw_buffer_size = 0;
   
   handle->m_own_reordered = 0;
   handle->m_reordered_buffer = 0;
   handle->m_reordered_buffer_size = 0;
   
   handle->m_own_compressed = 0;
   handle->m_compressed_buffer = 0;
   handle->m_compressed_buffer_size = 0;
   
   handle->m_calc_performance = 1; 
   
   handle->m_compression_ratio = 0;
   handle->m_encode_time = 0;
   handle->m_encode_rate = 0;
   handle->m_difference_time = 0;
   handle->m_difference_rate = 0;
   handle->m_reorder_time = 0;
   handle->m_reorder_rate = 0;
   handle->m_compress_time = 0;
   handle->m_compress_rate = 0;
   
   return XRIF_NOERROR;
}

// Set the difference method.
xrif_error_t xrif_set_difference_method( xrif_t handle,
                                         int difference_method
                                       )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_difference_method",  "can not configure null pointer.");
      return XRIF_ERROR_NULLPTR;
   }
   
   
   if( difference_method == XRIF_DIFFERENCE_NONE ) handle->m_difference_method = XRIF_DIFFERENCE_NONE;
   else if( difference_method == XRIF_DIFFERENCE_DEFAULT ) handle->m_difference_method = XRIF_DIFFERENCE_DEFAULT;
   else if( difference_method == XRIF_DIFFERENCE_PREVIOUS0 ) handle->m_difference_method = XRIF_DIFFERENCE_PREVIOUS0;
   else if( difference_method == XRIF_DIFFERENCE_PREVIOUS ) handle->m_difference_method = XRIF_DIFFERENCE_PREVIOUS;
   else if( difference_method == XRIF_DIFFERENCE_FIRST ) handle->m_difference_method = XRIF_DIFFERENCE_FIRST;
   else if( difference_method == XRIF_DIFFERENCE_PIXEL0 ) handle->m_difference_method = XRIF_DIFFERENCE_PIXEL0;
   else if( difference_method == XRIF_DIFFERENCE_PIXELL ) handle->m_difference_method = XRIF_DIFFERENCE_PIXELL;
   else
   {
      handle->m_difference_method = XRIF_DIFFERENCE_DEFAULT;
      XRIF_ERROR_PRINT("xrif_set_difference_method", "unrecognized difference method.  Setting default");
      return XRIF_ERROR_BADARG;
   }
   
   return XRIF_NOERROR;
}

// Set the reorder method.
xrif_error_t xrif_set_reorder_method( xrif_t handle,
                                      int reorder_method
                                    )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_reorder_method", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if( reorder_method == XRIF_REORDER_NONE ) handle->m_reorder_method = XRIF_REORDER_NONE;
   else if( reorder_method == XRIF_REORDER_DEFAULT ) handle->m_reorder_method = XRIF_REORDER_DEFAULT;
   else if( reorder_method == XRIF_REORDER_BYTEPACK ) handle->m_reorder_method = XRIF_REORDER_BYTEPACK;
   else if( reorder_method == XRIF_REORDER_BYTEPACK_RENIBBLE ) handle->m_reorder_method = XRIF_REORDER_BYTEPACK_RENIBBLE;
   else if( reorder_method == XRIF_REORDER_BITPACK ) handle->m_reorder_method = XRIF_REORDER_BITPACK;
   else
   {
      handle->m_reorder_method = XRIF_REORDER_DEFAULT;
      XRIF_ERROR_PRINT("xrif_set_reorder_method", "unrecognized reorder method.  Setting default");
      return XRIF_ERROR_BADARG;
   }
   
   return XRIF_NOERROR;
}

// Set the compress method with direction if needed.
xrif_error_t xrif_set_compress_method_direction( xrif_t handle,
                                                 xrif_int_t compress_method,
                                                 xrif_int_t compress_direction
                                               )
{
   int rv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_method", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   //If anything is setup, we reset and de-allocate.

   if( (rv = xrif_shutdown_zlib(handle)) < 0)
   {
      XRIF_ERROR_PRINT("xrif_set_compress_method_direction", "error from xrif_shutdown_zlib");
      return rv;
   }

   if( (rv = xrif_shutdown_zstd(handle)) < 0)
   {
      XRIF_ERROR_PRINT("xrif_set_compress_method_direction", "error from xrif_shutdown_zstd");
      return rv;
   }

   if(compress_direction != XRIF_DIRECTION_COMPRESS && compress_direction != XRIF_DIRECTION_DECOMPRESS)
   {
      handle->m_compress_method = XRIF_COMPRESS_DIRECTION_DEFAULT;
      XRIF_ERROR_PRINT("xrif_set_compress_method", "unrecognized compress direction.  Setting default");
   }

   handle->m_compress_direction = compress_direction;

   if( compress_method == XRIF_COMPRESS_NONE ) handle->m_compress_method = XRIF_COMPRESS_NONE;
   else if( compress_method == XRIF_COMPRESS_DEFAULT ) handle->m_compress_method = XRIF_COMPRESS_DEFAULT;
   else if( compress_method == XRIF_COMPRESS_LZ4 ) handle->m_compress_method = XRIF_COMPRESS_LZ4;
   else if( compress_method == XRIF_COMPRESS_LZ4HC ) handle->m_compress_method = XRIF_COMPRESS_LZ4HC;
   else if( compress_method == XRIF_COMPRESS_FASTLZ) handle->m_compress_method = XRIF_COMPRESS_FASTLZ;
   else if( compress_method == XRIF_COMPRESS_ZSTD) handle->m_compress_method = XRIF_COMPRESS_ZSTD;
   else if( compress_method == XRIF_COMPRESS_ZLIB) handle->m_compress_method = XRIF_COMPRESS_ZLIB;
   else
   {
      handle->m_compress_method = XRIF_COMPRESS_DEFAULT;
      XRIF_ERROR_PRINT("xrif_set_compress_method", "unrecognized compress method.  Setting default");
   }
   
   //Now do any necessary setups
   if( handle->m_compress_method == XRIF_COMPRESS_ZLIB) 
   {
      return xrif_setup_zlib(handle);
   }
   else if( handle->m_compress_method == XRIF_COMPRESS_ZSTD) 
   {
      return xrif_setup_zstd(handle);
   }
   else
   {
      return XRIF_NOERROR;
   }

}

// Set the compress method.
xrif_error_t xrif_set_compress_method( xrif_t handle,
                                       xrif_int_t compress_method
                                     )
{
   int rv;

   rv = xrif_set_compress_method_direction(handle, compress_method, XRIF_DIRECTION_COMPRESS);

   if(rv != XRIF_NOERROR)
   {
      XRIF_ERROR_PRINT("xrif_set_compress_method", "error in xrif_set_compress_method_direction");
   }

   return rv;
}

// Set the decompress method.
xrif_error_t xrif_set_decompress_method( xrif_t handle,
                                         xrif_int_t compress_method
                                       )
{
   int rv;

   rv = xrif_set_compress_method_direction(handle, compress_method, XRIF_DIRECTION_DECOMPRESS);

   if(rv != XRIF_NOERROR)
   {
      XRIF_ERROR_PRINT("xrif_set_decompress_method", "error in xrif_set_compress_method_direction");
   }

   return rv;
}

// Set the compression direction.
xrif_error_t xrif_set_compress_direction( xrif_t handle,
                                          xrif_int_t compress_direction
                                        )
{
   int rv;

   rv = xrif_set_compress_method_direction(handle, handle->m_compress_method, compress_direction);

   if(rv != XRIF_NOERROR)
   {
      XRIF_ERROR_PRINT("xrif_set_decompress_method", "error in xrif_set_compress_method_direction");
   }

   return rv;
}

// Calculate the minimum size of the raw buffer.
size_t xrif_min_raw_size(xrif_t handle)
{
    xrif_geometry geometry;
    if( xrif_geometry_get( handle, &geometry ) != XRIF_NOERROR )
        return 0;
    if( !handle->m_compress_on_raw )
        return geometry.m_raw_bytes;
    size_t compressed = xrif_min_compressed_size( handle );
    if( compressed == 0 )
        return 0;
    return compressed > geometry.m_raw_bytes ? compressed : geometry.m_raw_bytes;
}

// Calculate the minimum size of the reordered buffer.
size_t xrif_min_reordered_size(xrif_t handle)
{
    xrif_geometry geometry;
    if( xrif_geometry_get( handle, &geometry ) != XRIF_NOERROR )
        return 0;
    return xrif_reordered_bytes( &geometry, handle->m_frames, handle->m_reorder_method );
}

/// Calculate the minimum size of the compressed buffer.
size_t xrif_min_compressed_size(xrif_t handle)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size", "can not configure null pointer");
      return 0;
   }

   if(handle->m_compress_method == XRIF_COMPRESS_NONE)
   {
      return xrif_min_reordered_size(handle);
   }
   else if(handle->m_compress_method == XRIF_COMPRESS_LZ4)
   {
      return xrif_min_compressed_size_lz4(handle);   
   }
   else if(handle->m_compress_method == XRIF_COMPRESS_LZ4HC)
   {
      return xrif_min_compressed_size_lz4hc(handle);   
   }
   else if(handle->m_compress_method == XRIF_COMPRESS_FASTLZ)
   {
      return xrif_min_compressed_size_fastlz(handle);
   }
   else if(handle->m_compress_method == XRIF_COMPRESS_ZLIB)
   {
      return xrif_min_compressed_size_zlib(handle);
   }
   else if(handle->m_compress_method == XRIF_COMPRESS_ZSTD)
   {
      return xrif_min_compressed_size_zstd(handle);
   }

   XRIF_ERROR_PRINT("xrif_min_compressed_size", "unknown compression method");
   return 0;
}

// Set the raw data buffer to a pre-allocated pointer
xrif_error_t xrif_set_raw( xrif_t handle, void *buffer, size_t size )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( ( buffer == NULL ) != ( size == 0 ) )
        return XRIF_ERROR_INVALID_SIZE;
    size_t required = xrif_min_raw_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    if( size < required )
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    /* Passing the existing buffer back must not free it or lose its ownership. */
    if( buffer != handle->m_raw_buffer )
    {
        if( handle->m_own_raw )
            free( handle->m_raw_buffer );
        handle->m_raw_buffer = buffer;
        handle->m_own_raw    = 0;
    }
    handle->m_raw_buffer_size = size;
    return XRIF_NOERROR;
}

// Allocate the raw buffer based on the already set stream dimensions.
xrif_error_t xrif_allocate_raw( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_geometry_get( handle, &geometry );
    if( rv != XRIF_NOERROR )
        return rv;
    size_t required = xrif_min_raw_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    char *buffer = malloc( required );
    if( buffer == NULL )
        return XRIF_ERROR_MALLOC;
    if( handle->m_own_raw )
        free( handle->m_raw_buffer );
    handle->m_raw_buffer      = buffer;
    handle->m_raw_buffer_size = required;
    handle->m_own_raw         = 1;
    return XRIF_NOERROR;
}

// Set the reordered (working) data buffer to a pre-allocated pointer
xrif_error_t xrif_set_reordered( xrif_t handle, void *buffer, size_t size )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( ( buffer == NULL ) != ( size == 0 ) )
        return XRIF_ERROR_INVALID_SIZE;
    size_t required = xrif_min_reordered_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    if( size < required )
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    /* Passing the existing buffer back must not free it or lose its ownership. */
    if( buffer != handle->m_reordered_buffer )
    {
        if( handle->m_own_reordered )
            free( handle->m_reordered_buffer );
        handle->m_reordered_buffer = buffer;
        handle->m_own_reordered    = 0;
    }
    handle->m_reordered_buffer_size = size;
    return XRIF_NOERROR;
}

// Allocate the reordered buffer based on the already set stream dimensions.
xrif_error_t xrif_allocate_reordered( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_geometry_get( handle, &geometry );
    if( rv != XRIF_NOERROR )
        return rv;
    size_t required = xrif_min_reordered_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    char *buffer = malloc( required );
    if( buffer == NULL )
        return XRIF_ERROR_MALLOC;
    if( handle->m_own_reordered )
        free( handle->m_reordered_buffer );
    handle->m_reordered_buffer      = buffer;
    handle->m_reordered_buffer_size = required;
    handle->m_own_reordered         = 1;
    return XRIF_NOERROR;
}

xrif_error_t xrif_set_compressed( xrif_t handle, void *buffer, size_t size )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( ( buffer == NULL ) != ( size == 0 ) )
        return XRIF_ERROR_INVALID_SIZE;
    size_t required = xrif_min_compressed_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    if( size < required )
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    /* Passing the existing buffer back must not free it or lose its ownership. */
    if( buffer != handle->m_compressed_buffer )
    {
        if( handle->m_own_compressed )
            free( handle->m_compressed_buffer );
        handle->m_compressed_buffer = buffer;
        handle->m_own_compressed    = 0;
    }
    handle->m_compressed_buffer_size = size;
    return XRIF_NOERROR;
}

xrif_error_t xrif_allocate_compressed( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_geometry_get( handle, &geometry );
    if( rv != XRIF_NOERROR )
        return rv;
    size_t required = xrif_min_compressed_size( handle );
    if( required == 0 )
        return XRIF_ERROR_INVALID_SIZE;
    char *buffer = malloc( required );
    if( buffer == NULL )
        return XRIF_ERROR_MALLOC;
    if( handle->m_own_compressed )
        free( handle->m_compressed_buffer );
    handle->m_compressed_buffer      = buffer;
    handle->m_compressed_buffer_size = required;
    handle->m_own_compressed         = 1;
    return XRIF_NOERROR;
}

xrif_dimension_t xrif_width( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_width", "can not access a null pointer");
      return 0;
   }

   return handle->m_width;
}   

xrif_dimension_t xrif_height( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_height", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_height;
}

xrif_dimension_t xrif_depth( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_depth", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_depth;
}

xrif_dimension_t xrif_frames( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_frames", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_frames;
}   

xrif_int_t xrif_type_code( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_type_code", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_type_code;
}   

size_t xrif_data_size( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_data_size", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_data_size;
}   

size_t xrif_raw_size( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_raw_size", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_raw_size;
}   

size_t xrif_compressed_size( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compressed_size", "can not access a null pointer");
      return 0;
   }
   
   return handle->m_compressed_size;
}   

xrif_int_t xrif_difference_method( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_difference_method", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_difference_method;
}   

xrif_int_t xrif_reorder_method( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_reorder_method", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_reorder_method;
}

xrif_int_t xrif_compress_method( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_method", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_compress_method;
}

int xrif_omp_parallel( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_omp_parallel", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_omp_parallel;
}

int xrif_omp_numthreads( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_omp_numthreads", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_omp_numthreads;
}

/// Read an unsigned wire integer; legacy version 0 used native byte order.
static uint64_t xrif_header_get( const char *header, size_t offset, size_t width, int legacy )
{
    if( legacy )
    {
        if( width == 2 )
        {
            uint16_t value;
            memcpy( &value, header + offset, sizeof( value ) );
            return value;
        }
        uint32_t value;
        memcpy( &value, header + offset, sizeof( value ) );
        return value;
    }
    uint64_t value = 0;
    for( size_t i = 0; i < width; ++i )
    {
        value |= (uint64_t)(unsigned char)header[offset + i] << ( 8 * i );
    }
    return value;
}

/// Store an unsigned version-1 integer in little-endian order without alignment assumptions.
static void xrif_header_put( char *header, size_t offset, size_t width, uint64_t value )
{
    for( size_t i = 0; i < width; ++i )
    {
        header[offset + i] = (char)( value >> ( 8 * i ) );
    }
}

/// Decode a two's-complement method identifier without unsigned-to-signed narrowing.
static int xrif_header_method( const char *header, size_t offset, int legacy )
{
    uint32_t value = (uint32_t)xrif_header_get( header, offset, 2, legacy );
    return value <= INT16_MAX ? (int)value : (int)value - 65536;
}

xrif_error_t xrif_write_header_sized( char *header, size_t capacity, xrif_t handle )
{
    if( header == NULL || handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( capacity < XRIF_HEADER_SIZE )
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_geometry_get( handle, &geometry );
    if( rv != XRIF_NOERROR )
        return rv;
    if( handle->m_compressed_size > UINT64_MAX )
        return XRIF_ERROR_INVALID_SIZE;
    if( handle->m_difference_method < INT16_MIN || handle->m_difference_method > INT16_MAX ||
        handle->m_reorder_method < INT16_MIN || handle->m_reorder_method > INT16_MAX ||
        handle->m_compress_method < INT16_MIN || handle->m_compress_method > INT16_MAX )
        return XRIF_ERROR_INVALIDCONFIG;

    memset( header, 0, XRIF_HEADER_SIZE );
    memcpy( header, "xrif", 4 );
    xrif_header_put( header, 4, 4, XRIF_VERSION );
    xrif_header_put( header, 8, 4, XRIF_HEADER_SIZE );
    xrif_header_put( header, 12, 4, handle->m_width );
    xrif_header_put( header, 16, 4, handle->m_height );
    xrif_header_put( header, 20, 4, handle->m_depth );
    xrif_header_put( header, 24, 4, handle->m_frames );
    xrif_header_put( header, 28, 2, handle->m_type_code );
    xrif_header_put( header, 30, 2, (uint16_t)handle->m_difference_method );
    xrif_header_put( header, 32, 2, (uint16_t)handle->m_reorder_method );
    xrif_header_put( header, 34, 2, (uint16_t)handle->m_compress_method );
    /* Bytes 36..39 are reserved; the 64-bit payload length starts at byte 40. */
    xrif_header_put( header, 40, 8, handle->m_compressed_size );
    if( handle->m_compress_method == XRIF_COMPRESS_LZ4 )
        xrif_header_put( header, 48, 2, handle->m_lz4_acceleration );
    else if( handle->m_compress_method == XRIF_COMPRESS_LZ4HC )
        xrif_header_put( header, 48, 2, handle->m_lz4hc_level );
    else if( handle->m_compress_method == XRIF_COMPRESS_FASTLZ )
        xrif_header_put( header, 48, 2, handle->m_fastlz_level );
    else if( handle->m_compress_method == XRIF_COMPRESS_ZLIB )
    {
        xrif_header_put( header, 48, 1, handle->m_zlib_level );
        xrif_header_put( header, 49, 1, handle->m_zlib_strategy );
    }
    else if( handle->m_compress_method == XRIF_COMPRESS_ZSTD )
        xrif_header_put( header, 48, 4, (uint32_t)handle->m_zstd_level );
    return XRIF_NOERROR;
}

xrif_error_t xrif_write_header( char *header, xrif_t handle )
{
    return xrif_write_header_sized( header, XRIF_HEADER_SIZE, handle );
}

xrif_error_t xrif_read_header_sized( xrif_t handle, uint32_t *header_size, const char *header, size_t available )
{
    if( handle == NULL || header_size == NULL || header == NULL )
        return XRIF_ERROR_NULLPTR;
    if( available < 12 )
        return XRIF_ERROR_INSUFFICIENT_SIZE;
    if( memcmp( header, "xrif", 4 ) != 0 )
        return XRIF_ERROR_BADHEADER;
    uint64_t version = xrif_header_get( header, 4, 4, 0 );
    if( version > XRIF_VERSION )
        return XRIF_ERROR_WRONGVERSION;
    int      legacy = version == 0;
    uint32_t length = (uint32_t)xrif_header_get( header, 8, 4, legacy );
    if( length < ( legacy ? XRIF_HEADER_SIZE_V0 : XRIF_HEADER_SIZE ) )
        return XRIF_ERROR_BADHEADER;
    if( available < length )
        return XRIF_ERROR_INSUFFICIENT_SIZE;

    /* Stage metadata; failed parsing must not disturb buffers or codec contexts. */
    xrif_handle  parsed = *handle;
    xrif_error_t rv     = xrif_set_size( &parsed,
                                     (uint32_t)xrif_header_get( header, 12, 4, legacy ),
                                     (uint32_t)xrif_header_get( header, 16, 4, legacy ),
                                     (uint32_t)xrif_header_get( header, 20, 4, legacy ),
                                     (uint32_t)xrif_header_get( header, 24, 4, legacy ),
                                     (int)xrif_header_get( header, 28, 2, legacy ) );
    if( rv != XRIF_NOERROR )
        return rv;
    uint64_t compressed = xrif_header_get( header, legacy ? 36 : 40, legacy ? 4 : 8, legacy );
    if( compressed > SIZE_MAX )
        return XRIF_ERROR_INVALID_SIZE;
    parsed.m_compressed_size   = (size_t)compressed;
    parsed.m_difference_method = xrif_header_method( header, 30, legacy );
    parsed.m_reorder_method    = xrif_header_method( header, 32, legacy );
    parsed.m_compress_method   = xrif_header_method( header, 34, legacy );
    size_t options             = legacy ? 40 : 48;
    if( parsed.m_compress_method == XRIF_COMPRESS_LZ4 )
        parsed.m_lz4_acceleration = (int)xrif_header_get( header, options, 2, legacy );
    else if( parsed.m_compress_method == XRIF_COMPRESS_LZ4HC )
        parsed.m_lz4hc_level = (int)xrif_header_get( header, options, 2, legacy );
    else if( parsed.m_compress_method == XRIF_COMPRESS_FASTLZ )
        parsed.m_fastlz_level = (int)xrif_header_get( header, options, 2, legacy );
    else if( parsed.m_compress_method == XRIF_COMPRESS_ZLIB )
    {
        parsed.m_zlib_level    = (unsigned char)header[options];
        parsed.m_zlib_strategy = (unsigned char)header[options + 1];
    }
    else if( parsed.m_compress_method == XRIF_COMPRESS_ZSTD )
    {
        uint64_t value      = xrif_header_get( header, options, 4, legacy );
        parsed.m_zstd_level = value <= INT32_MAX ? (int32_t)value : (int32_t)( (int64_t)value - INT64_C( 4294967296 ) );
    }
    *handle      = parsed;
    *header_size = length;
    return XRIF_NOERROR;
}

xrif_error_t xrif_read_header( xrif_t handle, uint32_t *header_size, char *header )
{
    /* The historical API has no capacity; callers must provide a complete header. */
    return xrif_read_header_sized( handle, header_size, header, SIZE_MAX );
}

xrif_error_t xrif_encode( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  size_rv = xrif_validate_raw( handle, &geometry, 0 );
    if( size_rv != XRIF_NOERROR )
        return size_rv;

    if( handle == NULL )
    {
        XRIF_ERROR_PRINT( "xrif_encode", "can not use a null pointer" );
        return XRIF_ERROR_NULLPTR;
    }
   
   xrif_error_t rv;

   if( xrif_min_compressed_size( handle ) == 0 )
       return XRIF_ERROR_INVALID_SIZE;

   clock_gettime(CLOCK_REALTIME, &handle->m_ts_difference_start);
   
   if( handle->m_difference_method == XRIF_DIFFERENCE_NONE && handle->m_reorder_method == XRIF_REORDER_NONE && handle->m_compress_method == XRIF_COMPRESS_NONE)
   {
      //Set compressed size.
      if( !handle->m_compress_on_raw )
      {
          if( handle->m_compressed_buffer == NULL )
              return XRIF_ERROR_NOT_SETUP;
          if( handle->m_compressed_buffer_size < geometry.m_raw_bytes )
              return XRIF_ERROR_INSUFFICIENT_SIZE;
          memmove( handle->m_compressed_buffer, handle->m_raw_buffer, geometry.m_raw_bytes );
      }
      handle->m_compressed_size = geometry.m_raw_bytes;

      if(handle->m_calc_performance)
      {
         //So that we have some non-inifinities in the performance metrics.
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_reorder_start);
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_compress_start);
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_compress_done);
      }
      
      //but otherwise do nothing.
   }
   else
   {
      rv = xrif_difference( handle);
   
      if( rv != XRIF_NOERROR ) 
      {
         XRIF_ERROR_PRINT("xrif_encode", "error in xrif_difference");
         return rv;
      }
      
      clock_gettime(CLOCK_REALTIME, &handle->m_ts_reorder_start);
      
      rv = xrif_reorder(handle);
      
      if( rv != XRIF_NOERROR ) 
      {
         XRIF_ERROR_PRINT("xrif_encode", "error in xrif_reorder");
         return rv;
      }
      
      clock_gettime(CLOCK_REALTIME, &handle->m_ts_compress_start);
      
      rv = xrif_compress(handle);
      
      if( rv != XRIF_NOERROR ) 
      {
         XRIF_ERROR_PRINT("xrif_encode", "error in xrif_compress");
         return rv;
      }
      
      clock_gettime(CLOCK_REALTIME, &handle->m_ts_compress_done);
   }
   
   if(handle->m_calc_performance)
   {
      handle->m_compression_ratio = xrif_compression_ratio(handle);
      handle->m_encode_time = xrif_encode_time(handle);
      handle->m_encode_rate = xrif_encode_rate(handle);
      handle->m_difference_time = xrif_difference_time(handle);
      handle->m_difference_rate = xrif_difference_rate(handle);
      handle->m_reorder_time = xrif_reorder_time(handle);
      handle->m_reorder_rate = xrif_reorder_rate(handle);
      handle->m_compress_time = xrif_compress_time(handle);
      handle->m_compress_rate = xrif_compress_rate(handle);
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_decode( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  size_rv = xrif_validate_raw( handle, &geometry, 0 );
    if( size_rv != XRIF_NOERROR )
        return size_rv;

    if( handle == NULL )
    {
        XRIF_ERROR_PRINT( "xrif_encode", "can not use a null pointer" );
        return XRIF_ERROR_NULLPTR;
    }
   
   xrif_error_t rv;
   
   clock_gettime(CLOCK_REALTIME, &handle->m_ts_decompress_start);
   
   if( handle->m_difference_method == XRIF_DIFFERENCE_NONE && handle->m_reorder_method == XRIF_REORDER_NONE && handle->m_compress_method == XRIF_COMPRESS_NONE)
   {
       if( handle->m_compressed_size != geometry.m_raw_bytes )
           return XRIF_ERROR_INVALID_SIZE;
       if( !handle->m_compress_on_raw )
       {
           if( handle->m_compressed_buffer == NULL )
               return XRIF_ERROR_NOT_SETUP;
           if( handle->m_compressed_buffer_size < geometry.m_raw_bytes )
               return XRIF_ERROR_INSUFFICIENT_SIZE;
           memmove( handle->m_raw_buffer, handle->m_compressed_buffer, geometry.m_raw_bytes );
       }

      if(handle->m_calc_performance)
      {
         //So that we have some non-inifinities in the performance metrics.
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_unreorder_start);
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_undifference_start);
         clock_gettime(CLOCK_REALTIME, &handle->m_ts_undifference_done);
      }
      
      //but otherwise do nothing.
   }
   else
   {
      rv = xrif_decompress(handle);
      
      if( rv != XRIF_NOERROR ) 
      {
         fprintf(stderr, "xrif_decode: error returned by xrif_decompress\n");
         return rv;
      }
      clock_gettime(CLOCK_REALTIME, &handle->m_ts_unreorder_start);
      
      rv = xrif_unreorder(handle);
      
      if( rv != XRIF_NOERROR )
      {
         fprintf(stderr, "xrif_decode: error returned by xrif_unreorder\n");
         return rv;
      }

      clock_gettime(CLOCK_REALTIME, &handle->m_ts_undifference_start);
         
      rv = xrif_undifference(handle);
      
      if( rv != XRIF_NOERROR )
      {
         fprintf(stderr, "xrif_decode: error returned by xrif_undifference\n");
         return rv;
      }

      clock_gettime(CLOCK_REALTIME, &handle->m_ts_undifference_done);
   }
   
   return XRIF_NOERROR;
}

xrif_error_t xrif_difference( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  size_rv = xrif_validate_raw( handle, &geometry, 0 );
    if( size_rv != XRIF_NOERROR )
        return size_rv;

    if( handle == NULL )
    {
        XRIF_ERROR_PRINT( "xrif_difference", "can not use a null pointer" );
        return XRIF_ERROR_NULLPTR;
    }
   
   int method = handle->m_difference_method;
   
   if(method == 0) method = XRIF_DIFFERENCE_DEFAULT;
   
   switch( method )
   {
      case XRIF_DIFFERENCE_NONE:
         return XRIF_NOERROR;
      case XRIF_DIFFERENCE_PREVIOUS0:
         return xrif_difference_previous0(handle);
      case XRIF_DIFFERENCE_PREVIOUS:
         return xrif_difference_previous(handle);
      case XRIF_DIFFERENCE_FIRST:
         return xrif_difference_first(handle);
      case XRIF_DIFFERENCE_PIXEL0:
         return xrif_difference_pixel0(handle);
      case XRIF_DIFFERENCE_PIXELL:
         return xrif_difference_pixelL(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}

xrif_error_t xrif_undifference( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  size_rv = xrif_validate_raw( handle, &geometry, 0 );
    if( size_rv != XRIF_NOERROR )
        return size_rv;

    if( handle == NULL )
    {
        XRIF_ERROR_PRINT( "xrif_undifference", "can not use a null pointer" );
        return XRIF_ERROR_NULLPTR;
    }
   
   int method = handle->m_difference_method;
   
   if(method == 0) method = XRIF_DIFFERENCE_DEFAULT;
   
   switch( method )
   {
      case XRIF_DIFFERENCE_NONE:
         return XRIF_NOERROR;
      case XRIF_DIFFERENCE_PREVIOUS0:
         return xrif_undifference_previous0(handle);
      case XRIF_DIFFERENCE_PREVIOUS:
         return xrif_undifference_previous(handle);
      case XRIF_DIFFERENCE_FIRST:
         return xrif_undifference_first(handle);
      case XRIF_DIFFERENCE_PIXEL0:
         return xrif_undifference_pixel0(handle);
      case XRIF_DIFFERENCE_PIXELL:
         return xrif_undifference_pixelL(handle);
      default:
         return XRIF_ERROR_NOTIMPL;
   }
}

xrif_error_t xrif_reorder( xrif_t handle )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;

    int method = handle->m_reorder_method;

    if( method == 0 )
        method = XRIF_REORDER_DEFAULT;

    switch( method )
    {
    case XRIF_REORDER_NONE:
        return xrif_reorder_none( handle );
    case XRIF_REORDER_BYTEPACK:
        return xrif_reorder_bytepack( handle );
    case XRIF_REORDER_BYTEPACK_RENIBBLE:
        return xrif_reorder_bytepack_renibble( handle );
    case XRIF_REORDER_BITPACK:
        return xrif_reorder_bitpack( handle );
    default:
        return XRIF_ERROR_NOTIMPL;
    }
}

xrif_error_t xrif_unreorder( xrif_t handle )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;

    int method = handle->m_reorder_method;

    if( method == 0 )
        method = XRIF_REORDER_DEFAULT;

    switch( method )
    {
    case XRIF_REORDER_NONE:
        return xrif_unreorder_none( handle );
    case XRIF_REORDER_BYTEPACK:
        return xrif_unreorder_bytepack( handle );
    case XRIF_REORDER_BYTEPACK_RENIBBLE:
        return xrif_unreorder_bytepack_renibble( handle );
    case XRIF_REORDER_BITPACK:
        return xrif_unreorder_bitpack( handle );
    default:
        return XRIF_ERROR_NOTIMPL;
    }
}

/// Perform no re-ordering, simply copy raw to reordered.
xrif_error_t xrif_reorder_none( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_validate_reorder( handle, &geometry, 0, XRIF_REORDER_NONE );
    if( rv != XRIF_NOERROR )
        return rv;
    size_t npix = geometry.m_total_pixels;

    memcpy( handle->m_reordered_buffer, handle->m_raw_buffer, npix * handle->m_data_size );

    //Zero the rest of the reordered buffer.
    if( handle->m_reordered_buffer_size > npix * handle->m_data_size )
    {
        memset( handle->m_reordered_buffer + npix * handle->m_data_size,
                0,
                handle->m_reordered_buffer_size - npix * handle->m_data_size );
    }
   
   return XRIF_NOERROR;
}

//--------------------------------------------------------------------
//  none unreodering
//--------------------------------------------------------------------

/// Perform no unreordering, simply copy reordered to raw.
xrif_error_t xrif_unreorder_none( xrif_t handle )
{
    xrif_geometry geometry;
    xrif_error_t  rv = xrif_validate_reorder( handle, &geometry, 0, XRIF_REORDER_NONE );
    if( rv != XRIF_NOERROR )
        return rv;
    size_t npix = geometry.m_total_pixels;

    memcpy( handle->m_raw_buffer, handle->m_reordered_buffer, npix * handle->m_data_size );

    //Zero the rest of the raw buffer.
    if( handle->m_raw_buffer_size > npix * handle->m_data_size )
    {
        memset( handle->m_raw_buffer + npix * handle->m_data_size,
                0,
                handle->m_raw_buffer_size - npix * handle->m_data_size );
    }
   
   return XRIF_NOERROR;

}//xrif_error_t xrif_unreorder_none( xrif_t handle )

//==========================================================
//             Compression
//==========================================================

// Dispatch compression based on method
xrif_error_t xrif_compress( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   int method = handle->m_compress_method;
   
   if(method == 0) method = XRIF_COMPRESS_DEFAULT;
   
   switch( method )
   {
      case XRIF_COMPRESS_NONE:
         return xrif_compress_none(handle);
      case XRIF_COMPRESS_LZ4:
         return xrif_compress_lz4(handle);
      case XRIF_COMPRESS_LZ4HC:
         return xrif_compress_lz4hc(handle);
      case XRIF_COMPRESS_FASTLZ:
         return xrif_compress_fastlz(handle);
      case XRIF_COMPRESS_ZLIB:
         return xrif_compress_zlib(handle);
      case XRIF_COMPRESS_ZSTD:
         return xrif_compress_zstd(handle);
      default:
         XRIF_ERROR_PRINT("xrif_compress", "unknown compression method");
         return XRIF_ERROR_NOTIMPL;
   }
      
} //xrif_error_t xrif_compress( xrif_t handle )

// Dispatch decompression based on method
xrif_error_t xrif_decompress( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   int method = handle->m_compress_method;
   
   if(method == 0) method = XRIF_COMPRESS_DEFAULT;
   
   switch( method )
   {
      case XRIF_COMPRESS_NONE:
         return xrif_decompress_none(handle);
      case XRIF_COMPRESS_LZ4:
         return xrif_decompress_lz4(handle);
      case XRIF_COMPRESS_LZ4HC:
         return xrif_decompress_lz4hc(handle);
      case XRIF_COMPRESS_FASTLZ:
         return xrif_decompress_fastlz(handle);
      case XRIF_COMPRESS_ZLIB:
         return xrif_decompress_zlib(handle);
      case XRIF_COMPRESS_ZSTD:
         return xrif_decompress_zstd(handle);
      default:
         XRIF_ERROR_PRINT("xrif_decompress", "unknown compression method");
         return XRIF_ERROR_NOTIMPL;
   }
      
} //xrif_error_t xrif_decompress( xrif_t handle )

xrif_error_t xrif_compress_none( xrif_t handle )
{
    size_t       bytes = xrif_min_reordered_size( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, bytes, 0 );
    if( rv != XRIF_NOERROR )
        return rv;
    char *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    memcpy( buffer, handle->m_reordered_buffer, bytes );
    handle->m_compressed_size = bytes;
    return XRIF_NOERROR;
} //xrif_error_t xrif_compress_none( xrif_t handle )

xrif_error_t xrif_decompress_none( xrif_t handle )
{
    size_t       bytes = xrif_min_reordered_size( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, 0, 1 );
    if( rv != XRIF_NOERROR )
        return rv;
    if( handle->m_compressed_size != bytes )
        return XRIF_ERROR_INVALID_SIZE;
    char *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    memcpy( handle->m_reordered_buffer, buffer, bytes );
    return XRIF_NOERROR;
} //xrif_error_t xrif_decompress_none( xrif_t handle )

//==========================================================
//             Performance
//==========================================================

double xrif_compression_ratio( xrif_t handle )
{
   return ((double)handle->m_compressed_size)/((double)handle->m_raw_size);
}

double xrif_encode_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_compress_done, &handle->m_ts_difference_start);
}

double xrif_encode_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_encode_time(handle);
}
   
double xrif_reorder_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_reorder_start, &handle->m_ts_difference_start);
}

double xrif_difference_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) / xrif_difference_time(handle);
}

double xrif_difference_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_compress_start, &handle->m_ts_reorder_start);
}

double xrif_reorder_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_reorder_time(handle);
}

double xrif_compress_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_compress_done, &handle->m_ts_compress_start);
}

double xrif_compress_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_compress_time(handle);
}

double xrif_decode_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_undifference_done, &handle->m_ts_decompress_start);
}

double xrif_decode_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_decode_time(handle);
}
   
double xrif_undifference_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_undifference_done, &handle->m_ts_undifference_start);
}

double xrif_undifference_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) / xrif_undifference_time(handle);
}

double xrif_unreorder_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_undifference_start, &handle->m_ts_unreorder_start);
}

double xrif_unreorder_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_unreorder_time(handle);
}

double xrif_decompress_time( xrif_t handle )
{
   return xrif_ts_difference( &handle->m_ts_unreorder_start, &handle->m_ts_decompress_start);
}

double xrif_decompress_rate( xrif_t handle )
{
   return ((double) handle->m_raw_size) /xrif_decompress_time(handle);
}

size_t xrif_typesize( xrif_int_t type_code)
{
   switch( type_code )
   {
      case XRIF_TYPECODE_UINT8:
         return sizeof(uint8_t);
      case XRIF_TYPECODE_INT8:
         return sizeof(int8_t);
      case XRIF_TYPECODE_UINT16:
         return sizeof(uint16_t);
      case XRIF_TYPECODE_INT16:
         return sizeof(int16_t);
      case XRIF_TYPECODE_UINT32:
         return sizeof(uint32_t);
      case XRIF_TYPECODE_INT32:
         return sizeof(int32_t);
      case XRIF_TYPECODE_UINT64:
         return sizeof(uint64_t);
      case XRIF_TYPECODE_INT64:
         return sizeof(int64_t);
      case XRIF_TYPECODE_HALF:
         return sizeof(uint16_t);
      case XRIF_TYPECODE_FLOAT:
         return sizeof(float);
      case XRIF_TYPECODE_DOUBLE:
         return sizeof(double);
      case XRIF_TYPECODE_COMPLEX_FLOAT:
         return sizeof(float[2]);
      case XRIF_TYPECODE_COMPLEX_DOUBLE:
         return sizeof(double[2]);
      default:
         return 0;
   }
}

double xrif_ts_difference( struct timespec * ts1,
                           struct timespec * ts0
                         )
{
   return ((double)ts1->tv_sec) + ((double)ts1->tv_nsec)/1e9 - ((double)ts0->tv_sec) - ((double)ts0->tv_nsec)/1e9;
}

const char * xrif_difference_method_string( int diff_method )
{
   switch(diff_method)
   {
      case XRIF_DIFFERENCE_NONE:
         return "none";
      case XRIF_DIFFERENCE_PREVIOUS0:
         return "previous";
      case XRIF_DIFFERENCE_FIRST:
         return "first";
      default:
         return "unknown";
   }
}

const char * xrif_reorder_method_string( int reorder_method )
{
   switch(reorder_method)
   {
      case XRIF_REORDER_NONE:
         return "none";
      case XRIF_REORDER_BYTEPACK:
         return "bytepack";
      case XRIF_REORDER_BYTEPACK_RENIBBLE:
         return "bytepack w/ renibble";
      case XRIF_REORDER_BITPACK:
         return "bitpack";
      default:
         return "unknown";
   }
}

const char * xrif_compress_method_string( int compress_method )
{
   switch(compress_method)
   {
      case XRIF_COMPRESS_NONE:
         return "none";
      case XRIF_COMPRESS_LZ4:
         return "LZ4";
      default:
         return "unknown";
   }
}
