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



// Calculate the minimum size of the compressed buffer for zlib compression
size_t xrif_min_compressed_size_zlib(xrif_t handle)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_zlib", "can not configure null pointer");
      return 0;
   }

   size_t rsz = xrif_min_reordered_size(handle); 

   if(rsz == 0)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_zlib", "xrif_min_reordered_size failed");
      return 0;
   }

   if(handle->m_zlib_stream == NULL)
   {
      int rv;
      if( (rv = xrif_setup_zlib(handle)) < 0)
      {
         XRIF_ERROR_PRINT("xrif_min_compressed_size_zlib", "xrif_setup_zlib failed");
         return rv;
      }
   }

   return deflateBound(handle->m_zlib_stream, rsz);
}


xrif_error_t xrif_compress_zlib( xrif_t handle )
{
   char *m_compressed_buffer;
   size_t compressed_size;
   
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_zlib", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   if(handle->m_compress_direction != XRIF_DIRECTION_COMPRESS || handle->m_zlib_stream == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_zlib", "not configured for compression by zlib");
      return XRIF_ERROR_INVALIDCONFIG;
   }

   if(handle->m_compress_on_raw) 
   {
      m_compressed_buffer = handle->m_raw_buffer;
      compressed_size = handle->m_raw_buffer_size;
   }
   else 
   {
      m_compressed_buffer = handle->m_compressed_buffer;
      compressed_size = handle->m_compressed_buffer_size;
   }
   
   //zlib only takes ints for sizes
   int srcSize = xrif_min_reordered_size(handle); //This tells us how much memory is actually used by the reordering algorithm.

   if(compressed_size < xrif_min_compressed_size_zlib(handle))
   {
      XRIF_ERROR_PRINT("xrif_compress_zlib", "compressed_size is not large enough.");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   int rv = deflateReset(handle->m_zlib_stream);

   if(rv != Z_OK)
   {
      XRIF_ERROR_PRINT("xrif_compress_zlib", "zlib::deflateReset.");
      return XRIF_ERROR_LIBERR + rv;
   }


   handle->m_zlib_stream->avail_in = srcSize;
   handle->m_zlib_stream->next_in = handle->m_reordered_buffer;
   handle->m_zlib_stream->avail_out = compressed_size;
   handle->m_zlib_stream->next_out = m_compressed_buffer;
   handle->m_zlib_stream->data_type = Z_BINARY;

   rv = deflate(handle->m_zlib_stream, Z_FINISH);    

   if(rv != Z_STREAM_END)
   {
      XRIF_ERROR_PRINT("xrif_compress_zlib", "compression error from zlib.");
      return XRIF_ERROR_LIBERR + rv;;
   }
   
   handle->m_compressed_size = compressed_size - handle->m_zlib_stream->avail_out;

   return XRIF_NOERROR;
}

xrif_error_t xrif_decompress_zlib( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_compress_direction != XRIF_DIRECTION_DECOMPRESS || handle->m_zlib_stream == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "not configured for decompression by zlib");
      return XRIF_ERROR_INVALIDCONFIG;
   }

   char *m_compressed_buffer;
   
   if(handle->m_compress_on_raw) 
   {
      m_compressed_buffer = handle->m_raw_buffer;
   }
   else 
   {
      m_compressed_buffer = handle->m_compressed_buffer;
   }

   int rv = inflateReset(handle->m_zlib_stream);

   if(rv != Z_OK)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "zlib::inflateReset.");
      return XRIF_ERROR_LIBERR + rv;
   }

   handle->m_zlib_stream->avail_in = handle->m_compressed_size;
   handle->m_zlib_stream->next_in = m_compressed_buffer;
   handle->m_zlib_stream->avail_out = handle->m_reordered_buffer_size;
   handle->m_zlib_stream->next_out = handle->m_reordered_buffer;
   handle->m_zlib_stream->data_type = Z_BINARY;

   rv = inflate(handle->m_zlib_stream, Z_NO_FLUSH);

   if(rv != Z_STREAM_END)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "decompression error from zlib.");
      return XRIF_ERROR_LIBERR + rv;
   }

   int size_decomp =  handle->m_reordered_buffer_size - handle->m_zlib_stream->avail_out;

   if(size_decomp == 0)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "decompression failed.  check m_reordered_buffer_size, or possible corruption.");
      return (XRIF_ERROR_FAILURE);
   }
   
   //Make sure we have the correct amount of data
   if(xrif_min_reordered_size(handle) != size_decomp) 
   {
      XRIF_ERROR_PRINT("xrif_decompress_zlib", "size mismatch after decompression.");
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   return XRIF_NOERROR;

}
