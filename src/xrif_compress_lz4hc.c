/** \file xrif_compress_lz4hc.c
  * \brief Implementation of xrif LZ4HC compression
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

// Set the LZ4HC compression level
xrif_error_t xrif_set_lz4hc_level( xrif_t handle,      ///< [in/out] the xrif handle to be configured
                                   int32_t lz4hc_level ///< [in] LZ4HC compression level
                                 )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_lz4hc_level", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(lz4hc_level < LZ4HC_CLEVEL_MIN)
   {
      XRIF_ERROR_PRINT("xrif_set_lz4hc_level", "LZ4HC compression level can't be less than LZ4HC_CLEVEL_MIN.  Setting to LZ4HC_CLEVEL_MIN.");
      handle->m_lz4hc_level = LZ4HC_CLEVEL_MIN;
      return XRIF_ERROR_BADARG;
   }
   
   if(lz4hc_level > LZ4HC_CLEVEL_MAX ) //Max according to LZ4 docs
   {
      XRIF_ERROR_PRINT("xrif_set_lz4hc_level", "LZ4HC compression level can't be greater than LZ4HC_CLEVEL_MAX.  Setting to LZ4HC_CLEVEL_MAX.");
      handle->m_lz4hc_level = LZ4HC_CLEVEL_MAX;
      return XRIF_ERROR_BADARG;
   }
   
   handle->m_lz4hc_level = lz4hc_level;
   
   return XRIF_NOERROR;
}

xrif_int_t xrif_lz4hc_level( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_lz4hc_level", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_lz4hc_level;
}

// Calculate the minimum size of the compressed buffer for LZ4HC compression
size_t xrif_min_compressed_size_lz4hc(xrif_t handle)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_lz4hc", "can not configure null pointer");
      return 0;
   }

   size_t rsz = xrif_min_reordered_size(handle); 

   if(rsz == 0)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_lz4hc", "xrif_min_reordered_size failed");
      return 0;
   }

   //We compress the reordered buffer, and it can be the largest buffer.
   return LZ4_compressBound(rsz);
}


//Compress using LZ4HC
xrif_error_t xrif_compress_lz4hc( xrif_t handle )
{
   char *m_compressed_buffer;
   size_t compressed_size;
   
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_lz4hc", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
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
   
   //LZ4HC only takes ints for sizes
   int srcSize = xrif_min_reordered_size(handle); //This tells us how much memory is actually used by the reordering algorithm.
   
   if(compressed_size < xrif_min_compressed_size_lz4hc(handle))
   {
      XRIF_ERROR_PRINT("xrif_compress_lz4hc", "compressed_size is not large enough.");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }

   handle->m_compressed_size = LZ4_compress_HC(handle->m_reordered_buffer, m_compressed_buffer, srcSize, compressed_size, handle->m_lz4hc_level);
   
   if(handle->m_compressed_size == 0 )
   {
      XRIF_ERROR_PRINT("xrif_compress_lz4hc", "compression failed");
      return XRIF_ERROR_FAILURE;
   }
   
   return XRIF_NOERROR;
}

//Decompress using LZ4HC
xrif_error_t xrif_decompress_lz4hc( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_lz4hc", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
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
   
   int size_decomp = LZ4_decompress_safe (m_compressed_buffer, handle->m_reordered_buffer, handle->m_compressed_size, handle->m_reordered_buffer_size);

   if(size_decomp < 0)
   {
      XRIF_ERROR_PRINT("xrif_decompress_lz4hc", "error in LZ4_decompress_safe");
      return (XRIF_ERROR_LIBERR + size_decomp);
   }
   
   //Make sure we have the correct amount of data
   if(xrif_min_reordered_size(handle) != size_decomp) 
   {
      XRIF_ERROR_PRINT("xrif_decompress_lz4hc", "size mismatch after decompression.");
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   return XRIF_NOERROR;
}