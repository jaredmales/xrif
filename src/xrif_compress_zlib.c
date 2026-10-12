/** \file xrif_compress_zlib.c
  * \brief Implementation of xrif zlib compression
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup xrif_files
  */

/* This file is part of the xrif library.

Copyright (c) 2021 The Arizona Board of Regents on behalf of The
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

// Allocate and configure the `zlib` stream structure
xrif_error_t xrif_setup_zlib(xrif_t handle )
{
   int rv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_setup_zlib", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_zlib_stream != NULL)
   {
      if( (rv = xrif_shutdown_zlib(handle)) != XRIF_NOERROR)
      {
         XRIF_ERROR_PRINT("xrif_setup_zlib", "error from xrif_shutdown_zlib");
         return rv;
      }
   }

   handle->m_zlib_stream = (z_stream *) malloc( sizeof(z_stream) );

   if(handle->m_zlib_stream == NULL)
   {
      XRIF_ERROR_PRINT("xrif_setup_zlib", "error allocating z_stream");
      return XRIF_ERROR_MALLOC;
   }

   handle->m_zlib_stream->zalloc = Z_NULL;
   handle->m_zlib_stream->zfree = Z_NULL;
   handle->m_zlib_stream->opaque = Z_NULL;

   if(handle->m_compress_direction == XRIF_DIRECTION_COMPRESS)
   {
      rv = deflateInit2( handle->m_zlib_stream, handle->m_zlib_level, Z_DEFLATED, XRIF_ZLIB_WINDOWBITS, XRIF_ZLIB_MEMLEVEL, handle->m_zlib_strategy);

      if(rv != Z_OK)
      {
         XRIF_ERROR_PRINT("xrif_setup_zlib", "error from zlib::deflateInit2");
         return XRIF_ERROR_LIBERR + rv;
      }
   }
   else if(handle->m_compress_direction == XRIF_DIRECTION_DECOMPRESS)
   {
      rv = inflateInit2( handle->m_zlib_stream, XRIF_ZLIB_WINDOWBITS);
      if(rv != Z_OK)
      {
         XRIF_ERROR_PRINT("xrif_setup_zlib", "error from zlib::inflateInit2");
         return XRIF_ERROR_LIBERR + rv;
      }
   } 
   else
   {
      XRIF_ERROR_PRINT("xrif_setup_zlib", "invalid compression direction");
      return XRIF_ERROR_INVALIDCONFIG;
   }
   
   return XRIF_NOERROR;
}

// De-allocate the `zlib` stream structure
xrif_error_t xrif_shutdown_zlib(xrif_t handle )
{
   int rv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_shutdown_zlib", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_zlib_stream == NULL) return XRIF_NOERROR;

   if(handle->m_compress_direction == XRIF_DIRECTION_COMPRESS)
   {
      rv = deflateEnd(handle->m_zlib_stream);
      free(handle->m_zlib_stream);
      handle->m_zlib_stream = NULL;

      if(rv != Z_OK)
      {
         XRIF_ERROR_PRINT("xrif_shutdown_zlib", "error from zlib deflateEnd. possible leak.");
         return XRIF_ERROR_LIBERR + rv;
      }

   }
   else if(handle->m_compress_direction == XRIF_DIRECTION_DECOMPRESS)
   {
      rv = inflateEnd(handle->m_zlib_stream);
      free(handle->m_zlib_stream);
      handle->m_zlib_stream = NULL;

      if(rv != Z_OK)
      {
         XRIF_ERROR_PRINT("xrif_shutdown_zlib", "error from zlib deflateEnd. possible leak.");
         return XRIF_ERROR_LIBERR + rv;
      }
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_shutdown_zlib", "invalid compression direction");
      return XRIF_ERROR_INVALIDCONFIG;
   }
   
   return XRIF_NOERROR;

}

// Set the zlib compression level
xrif_error_t xrif_set_zlib_level( xrif_t handle,  
                                  xrif_int_t zlib_lev
                                )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_level", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   if(zlib_lev < 0)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_level", "level can not be less than zero. Setting default.");
      handle->m_zlib_level = XRIF_ZLIB_LEVEL_DEFAULT;
      return XRIF_ERROR_BADARG;
   }

   if(zlib_lev > 9)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_level", "level can not be greater than 9. Setting default.");
      handle->m_zlib_level = XRIF_ZLIB_LEVEL_DEFAULT;
      return XRIF_ERROR_BADARG;
   }

   handle->m_zlib_level = zlib_lev;

   return XRIF_NOERROR;
}

// Set the zlib strategy
xrif_error_t xrif_set_zlib_strategy( xrif_t handle,   
                                     xrif_int_t zlib_strat
                                   )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_strategy", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   if(zlib_strat < 0)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_strategy", "strategy can not be less than zero. Setting default.");
      handle->m_zlib_strategy = XRIF_ZLIB_STRATEGY_DEFAULT;
      return XRIF_ERROR_BADARG;
   }

   if(zlib_strat > 4)
   {
      XRIF_ERROR_PRINT("xrif_set_zlib_level", "strategy can not be greater than 4. Setting default.");
      handle->m_zlib_strategy = XRIF_ZLIB_STRATEGY_DEFAULT;
      return XRIF_ERROR_BADARG;
   }

   handle->m_zlib_strategy = zlib_strat;

   return XRIF_NOERROR;
}

// Get the zlib stream structure of the configured handle.
z_stream * xrif_zlib_stream( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zlib_stream", "can not access a null pointer");
      return NULL;
   }
   
   return handle->m_zlib_stream;
}

// Get the zlib compression level of the configured handle.
xrif_int_t xrif_zlib_level( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zlib_level", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_zlib_level;
}

// Get the zlib compression strategy of the configured handle.
xrif_int_t xrif_zlib_strategy( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zlib_dir", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_zlib_strategy;
}

// Calculate the minimum size of the compressed buffer for zlib compression.
size_t xrif_min_compressed_size_zlib(xrif_t handle)
{
    size_t bytes = xrif_min_reordered_size( handle );
    size_t conservative;
    /* Bound the library's internal additions before calling deflateBound. This
       conservative formula also covers its non-default strategy fallback. */
    if( bytes == 0 || bytes > ULONG_MAX || !xrif_size_add( bytes, bytes / 8 + ( bytes % 8 != 0 ), &conservative ) ||
        !xrif_size_add( conservative, bytes / 64 + ( bytes % 64 != 0 ), &conservative ) ||
        !xrif_size_add( conservative, 32, &conservative ) || conservative > ULONG_MAX )
        return 0;
    if( handle->m_zlib_stream == NULL && xrif_setup_zlib( handle ) != XRIF_NOERROR )
        return 0;
    uLong bound = deflateBound( handle->m_zlib_stream, (uLong)bytes );
    if( bound < bytes || bound > SIZE_MAX || bound > UINT64_MAX )
        return 0;
    return (size_t)bound;
}

/* Override only in the boundary-test target to exercise multiple windows with small buffers. */
#ifndef XRIF_ZLIB_WINDOW_SIZE
    #define XRIF_ZLIB_WINDOW_SIZE UINT_MAX
#endif
_Static_assert( XRIF_ZLIB_WINDOW_SIZE > 0 && XRIF_ZLIB_WINDOW_SIZE <= UINT_MAX, "invalid zlib window size" );

/// Process one zlib stream in bounded API windows, retaining size_t totals across calls.
/** Each call offers at most UINT_MAX bytes. The wire representation remains a single
  * zlib stream; no XRIF framing or chunks are added. A one-byte scratch destination
  * lets inflate consume a trailer after filling the expected output exactly.
  */
static xrif_error_t xrif_zlib_transfer( xrif_t      handle,
                                        const char *input,
                                        size_t      input_bytes,
                                        char       *output,
                                        size_t      output_bytes,
                                        int         decoding,
                                        size_t     *produced )
{
    z_stream *stream = handle->m_zlib_stream;
    int       rv     = decoding ? inflateReset( stream ) : deflateReset( stream );
    if( rv != Z_OK )
        return XRIF_ERROR_LIBERR;
    stream->avail_in  = 0;
    stream->avail_out = 0;
    stream->data_type = Z_BINARY;
    size_t consumed   = 0;
    *produced         = 0;
    unsigned char excess;
    for( ;; )
    {
        size_t in_left    = input_bytes - consumed;
        size_t out_left   = output_bytes - *produced;
        uInt   in_window  = (uInt)( in_left > XRIF_ZLIB_WINDOW_SIZE ? XRIF_ZLIB_WINDOW_SIZE : in_left );
        uInt   out_window = (uInt)( out_left > XRIF_ZLIB_WINDOW_SIZE ? XRIF_ZLIB_WINDOW_SIZE : out_left );
        int    scratch    = decoding && out_window == 0;
        if( out_window == 0 && !scratch )
            return XRIF_ERROR_INSUFFICIENT_SIZE;
        stream->next_in   = (Bytef *)( input + consumed );
        stream->avail_in  = in_window;
        stream->next_out  = scratch ? &excess : (Bytef *)( output + *produced );
        stream->avail_out = scratch ? 1 : out_window;
        uInt offered_out  = stream->avail_out;
        int  flush        = decoding ? Z_NO_FLUSH : ( in_left == in_window ? Z_FINISH : Z_NO_FLUSH );
        rv                = decoding ? inflate( stream, flush ) : deflate( stream, flush );
        size_t used_in    = in_window - stream->avail_in;
        size_t used_out   = offered_out - stream->avail_out;
        consumed += used_in;
        if( scratch && used_out != 0 )
            return XRIF_ERROR_INVALID_SIZE;
        *produced += used_out;
        if( rv == Z_STREAM_END )
        {
            if( consumed != input_bytes || ( decoding && *produced != output_bytes ) )
                return XRIF_ERROR_INVALID_SIZE;
            return XRIF_NOERROR;
        }
        if( rv != Z_OK || ( used_in == 0 && used_out == 0 ) )
            return XRIF_ERROR_LIBERR;
    }
}

xrif_error_t xrif_compress_zlib( xrif_t handle )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( handle->m_compress_direction != XRIF_DIRECTION_COMPRESS || handle->m_zlib_stream == NULL )
        return XRIF_ERROR_INVALIDCONFIG;
    size_t       bytes = xrif_min_reordered_size( handle );
    size_t       bound = xrif_min_compressed_size_zlib( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, bound, 0 );
    if( rv != XRIF_NOERROR )
        return rv;
    char  *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    size_t produced;
    rv = xrif_zlib_transfer( handle, handle->m_reordered_buffer, bytes, buffer, bound, 0, &produced );
    handle->m_compressed_size = rv == XRIF_NOERROR ? produced : 0;
    return rv;
}

xrif_error_t xrif_decompress_zlib( xrif_t handle )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    if( handle->m_compress_direction != XRIF_DIRECTION_DECOMPRESS || handle->m_zlib_stream == NULL )
        return XRIF_ERROR_INVALIDCONFIG;
    size_t       bytes = xrif_min_reordered_size( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, 0, 1 );
    if( rv != XRIF_NOERROR )
        return rv;
    char  *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    size_t produced;
    return xrif_zlib_transfer(
        handle, buffer, handle->m_compressed_size, handle->m_reordered_buffer, bytes, 1, &produced );
}
