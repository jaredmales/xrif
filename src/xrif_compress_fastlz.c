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
#include "xrif_size_internal.h"

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
xrif_int_t xrif_fastlz_level( xrif_t handle )
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
    size_t bytes = xrif_min_reordered_size( handle );
    size_t bound;
    if( bytes == 0 || bytes > INT_MAX || !xrif_size_add( bytes, bytes / 20 + 1, &bound ) )
        return 0;
    if( bound < 66 )
        bound = 66;
    return bound <= INT_MAX ? bound : 0;
}

// Compress using FastLZ
xrif_error_t xrif_compress_fastlz( xrif_t handle )
{
    size_t       bytes = xrif_min_reordered_size( handle );
    size_t       bound = xrif_min_compressed_size_fastlz( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, bound, 0 );
    if( rv != XRIF_NOERROR )
        return rv;
    char *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    /* Below FastLZ's documented minimum input size, emit the identical single
       literal run directly, avoiding its before-begin pointer arithmetic. */
    if( bytes < 16 )
    {
        if( handle->m_fastlz_level != 1 && handle->m_fastlz_level != 2 )
            return XRIF_ERROR_INVALIDCONFIG;
        buffer[0] = (char)( ( bytes - 1 ) | ( handle->m_fastlz_level == 2 ? 32 : 0 ) );
        memcpy( buffer + 1, handle->m_reordered_buffer, bytes );
        handle->m_compressed_size = bytes + 1;
        return XRIF_NOERROR;
    }
    int result = fastlz_compress_level( handle->m_fastlz_level, handle->m_reordered_buffer, (int)bytes, buffer );
    if( result <= 0 )
    {
        handle->m_compressed_size = 0;
        return XRIF_ERROR_FAILURE;
    }
    handle->m_compressed_size = (size_t)result;
    return XRIF_NOERROR;
}

// Decompress using FastLZ
xrif_error_t xrif_decompress_fastlz( xrif_t handle )
{
    if( handle == NULL )
        return XRIF_ERROR_NULLPTR;
    size_t bytes = xrif_min_reordered_size( handle );
    if( bytes > INT_MAX || handle->m_compressed_size > INT_MAX )
        return XRIF_ERROR_INVALID_SIZE;
    xrif_error_t rv = xrif_validate_codec( handle, bytes, 0, 1 );
    if( rv != XRIF_NOERROR )
        return rv;
    char *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    int   result = fastlz_decompress( buffer, (int)handle->m_compressed_size, handle->m_reordered_buffer, (int)bytes );
    if( result <= 0 )
        return XRIF_ERROR_LIBERR;
    return (size_t)result == bytes ? XRIF_NOERROR : XRIF_ERROR_INVALID_SIZE;
}