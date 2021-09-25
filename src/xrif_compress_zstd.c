/** \file xrif_compress_zstd.c
  * \brief Implementation of xrif zstd compression
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

// Set the zstd compression level
xrif_error_t xrif_set_zstd_level( xrif_t handle,   // [in/out] the xrif handle to be configured
                                  int32_t zstd_lev // [in] new zstd level
                                )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_zstd_level", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(zstd_lev < ZSTD_minCLevel())
   {
      XRIF_ERROR_PRINT("xrif_set_zstd_level", "FastLZ level can't be < ZSTD_minCLevel().  Setting to ZSTD_minCLevel().");
      zstd_lev = ZSTD_minCLevel();
   }
   
   if(zstd_lev > ZSTD_maxCLevel()) 
   {
      XRIF_ERROR_PRINT("xrif_set_zstd_level", "zstd level can't be greater than ZSTD_maxCLevel().  Setting to ZSTD_maxCLevel().");
      zstd_lev = ZSTD_maxCLevel();
   }
   
   if(zstd_lev == 0) zstd_lev = ZSTD_CLEVEL_DEFAULT;

   handle->m_zstd_level = zstd_lev;

   if(handle->m_zstd_cctx != NULL)
   {
      size_t zrv = ZSTD_CCtx_setParameter(handle->m_zstd_cctx, ZSTD_c_compressionLevel, handle->m_zstd_level);
      if(ZSTD_isError(zrv) != 0)
      {
         XRIF_ERROR_PRINT("xrif_set_zstd_level", "error from zstd::ZSTD_CCtx_setParameter setting level.");
         return XRIF_ERROR_LIBERR;// - ZSTD_getErrorCode(zrv); not yet available
      }
   }

   return XRIF_NOERROR;
}

// Get the zstd compression context structure of the configured handle.
ZSTD_CCtx * xrif_zstd_cctx( xrif_t handle /* [in] the xrif handle*/)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zstd_cctx", "can not access a null pointer");
      return NULL;
   }
   
   return handle->m_zstd_cctx;
}

// Get the zstd decompression context structure of the configured handle.
ZSTD_DCtx * xrif_zstd_dctx( xrif_t handle /* [in] the xrif handle*/)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zstd_dctx", "can not access a null pointer");
      return NULL;
   }
   
   return handle->m_zstd_dctx;
}

// Get the zstd compression level of the configured handle.
xrif_int_t xrif_zstd_level( xrif_t handle /* [in] the xrif handle*/)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_zstd_level", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_zstd_level;
}

// Allocate and configure the `zstd` context
xrif_error_t xrif_setup_zstd(xrif_t handle )
{
   xrif_error_t rv;
   size_t zrv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_setup_zstd", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_zstd_cctx != NULL || handle->m_zstd_dctx != NULL)
   {
      if( (rv = xrif_shutdown_zstd(handle)) != XRIF_NOERROR )
      {
         XRIF_ERROR_PRINT("xrif_setup_zstd", "error from xrif_shutdown_zstd");
         return rv;
      }
   }

   if(handle->m_compress_direction == XRIF_DIRECTION_COMPRESS)
   {
      handle->m_zstd_cctx = ZSTD_createCCtx();

      if(handle->m_zstd_cctx == NULL)
      {
         XRIF_ERROR_PRINT("xrif_setup_zstd", "error from zstd::ZSTD_createCCtx");
         return XRIF_ERROR_MALLOC;
      }

      zrv = ZSTD_CCtx_setParameter(handle->m_zstd_cctx, ZSTD_c_compressionLevel, handle->m_zstd_level);
      if(ZSTD_isError(zrv) != 0)
      {
         XRIF_ERROR_PRINT("xrif_setup_zstd", "error from zstd::ZSTD_CCtx_setParameter setting level.");
         return XRIF_ERROR_LIBERR;// - ZSTD_getErrorCode(zrv); not yet available
      }

   }
   else if(handle->m_compress_direction == XRIF_DIRECTION_DECOMPRESS)
   {
      handle->m_zstd_dctx = ZSTD_createDCtx();

      if(handle->m_zstd_dctx == NULL)
      {
         XRIF_ERROR_PRINT("xrif_setup_zstd", "error from zstd::ZSTD_createDCtx");
         return XRIF_ERROR_MALLOC;
      }
   }
   else
   {
      XRIF_ERROR_PRINT("xrif_setup_zstd", "invalid compression direction");
      return XRIF_ERROR_INVALIDCONFIG;
   }
   
   return XRIF_NOERROR;

}

/// De-allocate the `zstd` context
xrif_error_t xrif_shutdown_zstd(xrif_t handle )
{
   size_t rv;

   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_shutdown_zstd", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_zstd_cctx == NULL && handle->m_zstd_dctx == NULL) return XRIF_NOERROR;

   if(handle->m_zstd_cctx != NULL)
   {
      rv = ZSTD_freeCCtx(handle->m_zstd_cctx);
      handle->m_zstd_cctx = NULL;
      if(ZSTD_isError(rv) != 0)
      {
         XRIF_ERROR_PRINT("xrif_shutdown_zstd", "error from ZSTD_freeCCtx. possible leak.");
         return XRIF_ERROR_LIBERR;// - ZSTD_getErrorCode(rv); not yet available
      }
   }

   if(handle->m_zstd_dctx != NULL)
   {
      rv = ZSTD_freeDCtx(handle->m_zstd_dctx);
      handle->m_zstd_dctx = NULL;
      if(ZSTD_isError(rv) != 0)
      {
         XRIF_ERROR_PRINT("xrif_shutdown_zstd", "error from ZSTD_freeDCtx. possible leak.");
         return XRIF_ERROR_LIBERR;// - ZSTD_getErrorCode(rv); not yet available
      }
   }

   return XRIF_NOERROR;
}

size_t xrif_min_compressed_size_zstd(xrif_t handle)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_zstd", "can not configure null pointer");
      return 0;
   }

   size_t rsz = xrif_min_reordered_size(handle); 

   if(rsz == 0)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_zstd", "xrif_min_reordered_size failed");
      return 0;
   }

   //We compress the reordered buffer, and it can be the largest buffer.
   return ZSTD_compressBound(rsz);
}

//Compress using zstd
xrif_error_t xrif_compress_zstd( xrif_t handle )
{
   char *m_compressed_buffer;
   size_t compressed_size;
   
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_zstd", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   if(handle->m_compress_direction != XRIF_DIRECTION_COMPRESS || handle->m_zstd_cctx == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_zstd", "not configured for compression by zstd");
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
   
   size_t srcSize = xrif_min_reordered_size(handle); //This tells us how much memory is actually used by the reordering algorithm.
   
   if(compressed_size < xrif_min_compressed_size_zstd(handle))
   {
      XRIF_ERROR_PRINT("xrif_compress_zstd", "compressed_size is not large enough.");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   size_t zrv = ZSTD_compress2(handle->m_zstd_cctx, m_compressed_buffer, compressed_size, handle->m_reordered_buffer, srcSize);
   
   if(ZSTD_isError(zrv))
   {
      handle->m_compressed_size = 0;
      XRIF_ERROR_PRINT("xrif_compress_zstd", "compression failed");
      return XRIF_ERROR_FAILURE;
   }

   handle->m_compressed_size = zrv;
   
   return XRIF_NOERROR;
}

//Decompress using zstd
xrif_error_t xrif_decompress_zstd( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zstd", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(handle->m_compress_direction != XRIF_DIRECTION_DECOMPRESS || handle->m_zstd_dctx == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_zstd", "not configured for decompression by zstd");
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
   
   size_t size_decomp = ZSTD_decompressDCtx(handle->m_zstd_dctx, handle->m_reordered_buffer, handle->m_reordered_buffer_size, m_compressed_buffer, handle->m_compressed_size);

   if( ZSTD_isError(size_decomp))
   {
      XRIF_ERROR_PRINT("xrif_decompress_zstd", "error in ZSTD_decompressDCtx");
      return XRIF_ERROR_LIBERR;
   }
   
   //Make sure we have the correct amount of data
   if(xrif_min_reordered_size(handle) != size_decomp) 
   {
      XRIF_ERROR_PRINT("xrif_decompress_zstd", "size mismatch after decompression.");
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   return XRIF_NOERROR;
}
