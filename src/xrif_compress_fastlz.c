/** \file xrif_compress_fastlz.c
  * \brief Implementation of xrif FastLZ compression
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

// Set the FastLZ compression level
xrif_error_t xrif_set_fastlz_level( xrif_t handle,     // [in/out] the xrif handle to be configured
                                    int32_t fastlz_lev // [in] FastLZ level
                                  )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_fastlz_level", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(fastlz_lev < 1)
   {
      XRIF_ERROR_PRINT("xrif_set_fastlz_level", "FastLZ level can't be < 1.  Setting to 1.");
      handle->m_fastlz_level = 1;
      return XRIF_ERROR_BADARG;
   }
   
   if(fastlz_lev > 2) 
   {
      XRIF_ERROR_PRINT("xrif_set_fastlz_level", "FastLZ level can't be greater than 2.  Setting to 2.");
      handle->m_fastlz_level = 2;
      return XRIF_ERROR_BADARG;
   }
   
   handle->m_fastlz_level = fastlz_lev;
   
   return XRIF_NOERROR;
}

// Get the FastLZ compression level of the configured handle.
int xrif_fastlz_level( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_fastlz_level", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_fastlz_level;
}

// Calculate the minimum size of the compressed buffer for FastLZ compression
size_t xrif_min_compressed_size_fastlz(xrif_t handle)
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_fastlz", "can not configure null pointer");
      return 0;
   }

   size_t rsz = xrif_min_reordered_size(handle); 

   if(rsz == 0)
   {
      XRIF_ERROR_PRINT("xrif_min_compressed_size_fastlz", "xrif_min_reordered_size failed");
      return 0;
   }

   //FastLZ wants 5% more than the input size.  We compress the reordered buffer, and it can be the largest buffer.
   return 1.05*rsz + 1; //make sure we take the next one up.
}

// Compress using FastLZ
xrif_error_t xrif_compress_fastlz( xrif_t handle )
{
   char *compressed_buffer;
   size_t compressed_size;
   
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_compress_fastlz", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }

   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
      compressed_size = handle->raw_buffer_size;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
      compressed_size = handle->compressed_buffer_size;
   }
   
   //FastLZ only takes ints for sizes
   int srcSize = xrif_min_reordered_size(handle); //This tells us how much memory is actually used by the reordering algorithm.

   if(compressed_size < xrif_min_compressed_size_fastlz(handle))
   {
      XRIF_ERROR_PRINT("xrif_compress_fastlz", "compressed_size is not large enough.");
      return XRIF_ERROR_INSUFFICIENT_SIZE;
   }


   handle->m_compressed_size = fastlz_compress_level(handle->m_fastlz_level, handle->reordered_buffer, srcSize, compressed_buffer);
   
   if(handle->m_compressed_size == 0 )
   {
      XRIF_ERROR_PRINT("xrif_compress_fastlz", "compression failed.");
      return XRIF_ERROR_FAILURE;
   }
   
   
   return XRIF_NOERROR;
}

// Decompress using FastLZ
xrif_error_t xrif_decompress_fastlz( xrif_t handle )
{
   if(handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_decompress_fastlz", "can not use a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   char *compressed_buffer;
   
   if(handle->compress_on_raw) 
   {
      compressed_buffer = handle->raw_buffer;
   }
   else 
   {
      compressed_buffer = handle->compressed_buffer;
   }
   
   int size_decomp = fastlz_decompress(compressed_buffer, handle->m_compressed_size, handle->reordered_buffer, handle->reordered_buffer_size);

   if(size_decomp == 0)
   {
      XRIF_ERROR_PRINT("xrif_decompress_fastlz", "decompression failed.  check reordered_buffer_size, or possible corruption.");
      return (XRIF_ERROR_FAILURE);
   }
   
   //Make sure we have the correct amount of data
   if(xrif_min_reordered_size(handle) != size_decomp) 
   {
      XRIF_ERROR_PRINT("xrif_decompress_fastlz", "size mismatch after decompression.");
      return XRIF_ERROR_INVALID_SIZE;
   }
   
   return XRIF_NOERROR;
}