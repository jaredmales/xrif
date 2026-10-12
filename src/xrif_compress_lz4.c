/** \file xrif_compress_lz4.c
  * \brief Implementation of xrif LZ4 compression
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

// Set the LZ4 acceleration parameter
xrif_error_t xrif_set_lz4_acceleration( xrif_t handle,    // [in/out] the xrif handle to be configured
                                        int32_t lz4_accel // [in] LZ4 acceleration parameter, \>=1, higher is faster with less comporession.
                                      )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_set_lz4_acceleration", "can not configure null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   if(lz4_accel < XRIF_LZ4_ACCELERATION_MIN)
   {
      XRIF_ERROR_PRINT("xrif_set_lz4_acceleration", "LZ4 acceleration can't be less than XRIF_LZ4_ACCELERATION_MIN.  Setting to XRIF_LZ4_ACCELERATION_MIN.");
      handle->m_lz4_acceleration = XRIF_LZ4_ACCELERATION_MIN;
      return XRIF_ERROR_BADARG;
   }
   
   if(lz4_accel > XRIF_LZ4_ACCELERATION_MAX) //Max according to LZ4 docs
   {
      XRIF_ERROR_PRINT("xrif_set_lz4_acceleration", "LZ4 acceleration can't be greater than XRIF_LZ4_ACCELERATION_MAX.  Setting to XRIF_LZ4_ACCELERATION_MAX.");
      handle->m_lz4_acceleration = XRIF_LZ4_ACCELERATION_MAX;
      return XRIF_ERROR_BADARG;
   }
   
   handle->m_lz4_acceleration = lz4_accel;
   
   return XRIF_NOERROR;
}

// Get the current value of the LZ4 acceleration parameter
xrif_int_t xrif_lz4_acceleration( xrif_t handle )
{
   if( handle == NULL)
   {
      XRIF_ERROR_PRINT("xrif_lz4_acceleration", "can not access a null pointer");
      return XRIF_ERROR_NULLPTR;
   }
   
   return handle->m_lz4_acceleration;
}

// Calculate the minimum size of the compressed buffer for LZ4 compression
size_t xrif_min_compressed_size_lz4(xrif_t handle)
{
    size_t bytes = xrif_min_reordered_size( handle );
    if( bytes == 0 || bytes > LZ4_MAX_INPUT_SIZE )
        return 0;
    return (size_t)LZ4_compressBound( (int)bytes );
}


//Compress using LZ4
xrif_error_t xrif_compress_lz4( xrif_t handle )
{
    size_t       bytes = xrif_min_reordered_size( handle );
    size_t       bound = xrif_min_compressed_size_lz4( handle );
    xrif_error_t rv    = xrif_validate_codec( handle, bytes, bound, 0 );
    if( rv != XRIF_NOERROR )
        return rv;
    char *buffer = handle->m_compress_on_raw ? handle->m_raw_buffer : handle->m_compressed_buffer;
    int   result =
        LZ4_compress_fast( handle->m_reordered_buffer, buffer, (int)bytes, (int)bound, handle->m_lz4_acceleration );
    if( result <= 0 )
    {
        handle->m_compressed_size = 0;
        return XRIF_ERROR_FAILURE;
    }
    handle->m_compressed_size = (size_t)result;
    return XRIF_NOERROR;
}

//Decompress using LZ4
xrif_error_t xrif_decompress_lz4( xrif_t handle )
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
    int result = LZ4_decompress_safe( buffer, handle->m_reordered_buffer, (int)handle->m_compressed_size, (int)bytes );
    if( result < 0 )
        return XRIF_ERROR_LIBERR;
    return (size_t)result == bytes ? XRIF_NOERROR : XRIF_ERROR_INVALID_SIZE;
}
